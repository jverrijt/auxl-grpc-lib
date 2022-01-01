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
Descriptor::Descriptor() { }

/**
 This constructs the Descriptor and sets up the cluster of classes that generate the protobuf messages.
 */
std::shared_ptr<Descriptor> Descriptor::create_descriptor(std::vector<std::string> proto_files, Connection* connection)
{
    auto descr = std::shared_ptr<Descriptor>(new Descriptor());
    
    descr->db_ = std::unique_ptr<SimpleDescriptorDatabase>(new SimpleDescriptorDatabase());

    if (proto_files.size() > 0) {
        descriptors_from_proto_files(proto_files, descr->db_.get());
    }

    if (connection != NULL) {
        descriptors_from_reflect(*connection, descr->db_.get());
    }
    
    descr->pool_ = std::unique_ptr<DescriptorPool>(new DescriptorPool(descr->db_.get()));
    descr->factory_ = std::unique_ptr<DynamicMessageFactory>(new DynamicMessageFactory(descr->pool_.get()));
    
    return descr;
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
    auto stat = util::MessageToJsonString(message, &output, jsonPrintOptions);
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
        util::JsonParseOptions opts;
        
        opts.ignore_unknown_fields = true;
        
        auto stat = util::JsonStringToMessage(json, msg.get(), opts);
        
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
            util::MessageToJsonString(*proto, &output, options);
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



} // ns grpc
} // ns auxl
