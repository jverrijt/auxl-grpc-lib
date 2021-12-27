
#include "template.h"
#include <nlohmann/json.hpp>

#include <google/protobuf/util/json_util.h>
#include <google/protobuf/compiler/importer.h>
#include <google/protobuf/dynamic_message.h>
#include "../modules/grpc/test/cpp/util/proto_reflection_descriptor_database.h"

#define ADD_VAL(_type, val) if (field->is_repeated()) {\
        refl->Add##_type(message, field, val);\
    } else {\
        refl->Set##_type(message, field, val);\
    }\

using namespace nlohmann;
using namespace google::protobuf;

namespace auxl {
namespace grpc {


/**
 Create a message with some sensible defaults
 */
Message* build_message(const Descriptor* descriptor, DynamicMessageFactory* factory, int depth = 0, int max_depth = 1)
{
    auto message = factory->GetPrototype(descriptor)->New();
    auto refl = message->GetReflection();
       
    // Provide a default value for timestamps set to the current time
    if (descriptor->well_known_type() == Descriptor::WELLKNOWNTYPE_TIMESTAMP) {
        refl->SetInt64(message, descriptor->FindFieldByName("seconds"), time(NULL));
        refl->SetInt32(message, descriptor->FindFieldByName("nanos"), 0);
        return message;
    }
    
    for (int i = 0; i < descriptor->field_count(); i++) {
        auto field = descriptor->field(i);
        
        if (field->type() == FieldDescriptor::TYPE_MESSAGE && depth < max_depth) {
            auto sub_message = build_message(field->message_type(), factory, depth + 1);
            
            if (sub_message) {
                if (field->is_repeated()) {
                    refl->AddAllocatedMessage(message, field, sub_message);
                } else {
                    refl->SetAllocatedMessage(message, sub_message, field);
                }
            }
            
        } else {
            switch (field->type()) {
                case FieldDescriptor::TYPE_STRING:
                    // This makes map types a bit more legible
                    if (descriptor->map_key() == field) {
                        ADD_VAL(String, "Key")
                    } else if (descriptor->map_value() == field) {
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
 1. Look up the given message name in the given, by this tool generated, JSON descriptor
 2. If found, convert back into a FileDescriptorProto
 3. Construct a message
 */
void create_template_message(std::string message_name, std::string descriptor) 
{
    // Find the message in the given descriptor
    json o = json::parse(descriptor);
    google::protobuf::SimpleDescriptorDatabase descr_db;
    
    for (int i = 0; i < o.size(); i++) {
        auto proto = new google::protobuf::FileDescriptorProto();
        google::protobuf::util::JsonStringToMessage(o[i].dump(-1), proto);
        
        descr_db.Add(*proto);
        
        delete proto;
    }
    
    google::protobuf::DescriptorPool pool(&descr_db);
    google::protobuf::DynamicMessageFactory dynamic_factory(&pool);
    
    auto descr = pool.FindMessageTypeByName(message_name);

    auto template_message = build_message(descr, &dynamic_factory);
    
    util::JsonPrintOptions jsonPrintOptions;

    jsonPrintOptions.add_whitespace = true;
    jsonPrintOptions.always_print_primitive_fields = true;
    jsonPrintOptions.always_print_enums_as_ints = false;
    
    std::string output;
    util::MessageToJsonString(*template_message, &output, jsonPrintOptions);
    
    delete template_message;
    
    std::cout << output;
}

} // ns grpc
} // ns auxl
