/*
 *
 * Copyright 2022 Joost Verrijt.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "descriptor.hpp"
#include "parser.hpp"
#include <google/protobuf/dynamic_message.h>

#define ADD_VAL(_type, val) if (field->is_repeated()) {\
        refl->Add##_type(message.get(), field, val);\
    } else {\
        refl->Set##_type(message.get(), field, val);\
    }\

using namespace google::protobuf;

namespace auxl {
namespace grpc {

/**
 */
Descriptor::Descriptor(std::vector<std::string> proto_files, const Connection* connection)
{
    error_collector_ = create_error_collector();
    db_ = std::unique_ptr<SimpleDescriptorDatabase>(new SimpleDescriptorDatabase());

    if (proto_files.size() > 0) {
        descriptors_from_proto_files(proto_files, db_.get(), error_collector_);
    }

    if (connection != NULL) {
        descriptors_from_reflect(*connection, db_.get(), error_collector_);
    }
    
    pool_ = std::unique_ptr<DescriptorPool>(new DescriptorPool(db_.get()));
    factory_ = std::unique_ptr<DynamicMessageFactory>(new DynamicMessageFactory(pool_.get()));
}

/**
 */
Descriptor::Descriptor()
{
    error_collector_ = create_error_collector();
}

/**
 */
Descriptor::~Descriptor()
{
    error_collector_free(error_collector_);
}

/**
 Messages are dynamically allocated
 */
std::shared_ptr<google::protobuf::Message> Descriptor::create_message(const std::string& message_type_name)
{
    std::shared_ptr<google::protobuf::Message> msg;
    auto descr = pool_->FindMessageTypeByName(message_type_name);
    
    if (descr != nullptr) {
        msg = build_message(*descr, *factory_);
    }
    
    return msg;
}

/**
 */
std::string Descriptor::message_to_json(const Message& message, google::protobuf::util::JsonPrintOptions jsonPrintOptions)
{
    std::string output;
    auto stat = ::util::MessageToJsonString(message, &output, jsonPrintOptions);
    return output;
}

/**
 */
std::shared_ptr<Message> Descriptor::message_from_json(const std::string& message_type_name, const std::string& json)
{
    std::shared_ptr<Message> msg;
    auto descr = pool_->FindMessageTypeByName(message_type_name);
    
    if (descr != nullptr) {
        msg = std::shared_ptr<Message>(factory_->GetPrototype(descr)->New());
        ::util::JsonParseOptions opts;
        
        opts.ignore_unknown_fields = true;
        
        auto stat = ::util::JsonStringToMessage(json, msg.get(), opts);
        
        if (!stat.ok()) {
            // throw an error?
            std::cerr << "Unable to convert JSON to message (type: " << message_type_name << ")." << std::endl;
        }
    }
    
    return msg;
}

/**
 */
const MethodDescriptor* Descriptor::get_method_descriptor(const std::string& package_service_method)
{
    auto splitted = util::split_service_method(package_service_method);
        
    bool is_valid_method_string = std::get<0>(splitted);

    if (!is_valid_method_string) {
        return nullptr;
    }
    
    std::string package_service = std::get<1>(splitted);
    std::string method = std::get<2>(splitted);
    
    auto service_descr = pool_->FindServiceByName(package_service);
    
    if (service_descr == nullptr) {
        return nullptr;
    }
    
    auto method_descr = service_descr->FindMethodByName(method);
    
    if (method_descr == nullptr) {
        return nullptr;
    }
    
    return method_descr;
}

/**
 */
std::string Descriptor::to_json(google::protobuf::util::JsonPrintOptions options)
{
    std::vector<std::string> file_names;
    db_->FindAllFileNames(&file_names);
    
    std::string jsonOutput;
    jsonOutput += "[";

    for (std::string fn : file_names) {
        const auto proto = new FileDescriptorProto();

        if (db_->FindFileByName(fn, proto)) {
            std::string output;
            ::util::MessageToJsonString(*proto, &output, options);
            jsonOutput += output + ",";
        }
        
        delete proto;
    }
    
    // Remove trailing comma
    if (!jsonOutput.empty() && jsonOutput.find_last_of(",") == jsonOutput.length() - 1) {
        jsonOutput.pop_back();
    }

    jsonOutput += "]";

    return jsonOutput;
}

/**
 Create a message with some sensible defaults
 */
std::shared_ptr<Message> Descriptor::build_message(const google::protobuf::Descriptor& descriptor,
                                                   DynamicMessageFactory& factory, int depth, int max_depth)
{
    std::shared_ptr<Message> message(factory.GetPrototype(&descriptor)->New());
    
    auto refl = message->GetReflection();
       
    // Provide a default value for timestamps set to the current time
    if (descriptor.well_known_type() == google::protobuf::Descriptor::WELLKNOWNTYPE_TIMESTAMP) {
        refl->SetInt64(message.get(), descriptor.FindFieldByName("seconds"), time(NULL));
        refl->SetInt32(message.get(), descriptor.FindFieldByName("nanos"), 0);
        return message;
    }
    
    for (int i = 0; i < descriptor.field_count(); i++) {
        auto field = descriptor.field(i);
        
        if (field->type() == FieldDescriptor::TYPE_MESSAGE && depth < max_depth) {
            auto sub_message = build_message(*field->message_type(), factory, depth + 1);
            
            if (sub_message) {
                if (field->is_repeated()) {
                    refl->AddMessage(message.get(), field)->CopyFrom(*sub_message);
                } else {
                    refl->MutableMessage(message.get(), field)->CopyFrom(*sub_message);
                }
            }
        } else {
            switch (field->type()) {
                case FieldDescriptor::TYPE_STRING:
                    // This makes map types a bit more legible
                    if (descriptor.map_key() == field) {
                        ADD_VAL(String, "Key")
                    } else if (descriptor.map_value() == field) {
                        ADD_VAL(String, "Val")
                    }
                    break;
                default:
                    break;
            }
        }
    }
    
    return message;
}

/**
 */
bool Descriptor::load_json(const std::string& json)
{
    auto db = parse_descriptors(json);
    
    if (db.get() != nullptr) {
        db_ = std::move(db);
        pool_ = std::unique_ptr<DescriptorPool>(new DescriptorPool(db_.get()));
        factory_ = std::unique_ptr<DynamicMessageFactory>(new DynamicMessageFactory(pool_.get()));
        
        return true;
    }
    
    return false;
}

/**
 */
const AuxlGRPCErrorCollector* Descriptor::get_error_collector()
{
    return error_collector_;
}


} // ns grpc
} // ns auxl
