//
//  descriptor.cpp
//  auxl_grpc_cli
//
//  Created by Joost Verrijt on 31/12/2021.
//

#include "descriptor.hpp"
#include "parser.h"
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
Descriptor::~Descriptor()
{
    error_collector_free(error_collector_);
}

/**
 Messages are dynamically allocated
 */
std::shared_ptr<google::protobuf::Message> Descriptor::create_message(std::string message_type_name)
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
std::string Descriptor::message_to_json(Message& message, google::protobuf::util::JsonPrintOptions jsonPrintOptions)
{
    std::string output;
    auto stat = ::util::MessageToJsonString(message, &output, jsonPrintOptions);
    return output;
}

/**
 */
std::shared_ptr<Message> Descriptor::message_from_json(std::string message_type_name, std::string json)
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
            std::cerr << "ERRORx:" << std::endl;
        }
    }
    
    return msg;
}

/**
 */
const MethodDescriptor* Descriptor::get_method_descriptor(std::string service, std::string method)
{
    auto service_descr = pool_->FindServiceByName(service);
    
    if (service_descr == nullptr) {
        throw -1;
    }
    
    auto method_descr = service_descr->FindMethodByName(method);
    
    if (method_descr == nullptr) {
        throw -1;
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
    }

    // Remove trailing comma
    if (!jsonOutput.empty()) {
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
const AuxlGRPCErrorCollector* Descriptor::get_error_collector()
{
    return error_collector_;
}


} // ns grpc
} // ns auxl
