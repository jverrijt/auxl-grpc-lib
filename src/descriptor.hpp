//
//  descriptor.hpp
//  auxl_grpc_cli
//
//  Created by Joost Verrijt on 31/12/2021.
//

#ifndef descriptor_hpp
#define descriptor_hpp

#include "../modules/grpc/test/cpp/util/proto_reflection_descriptor_database.h"
#include <google/protobuf/compiler/importer.h>
#include <google/protobuf/util/json_util.h>
#include "error_collector.h"

#include "connection.h"

#include <stdio.h>
#include <string>
#include <memory>

namespace auxl {
namespace grpc {

// These json options work best when deserializing proto descriptors.
// For convenience, set always_print_primitive_fields to true when serializing messages
inline google::protobuf::util::JsonPrintOptions default_json_options()
{
    google::protobuf::util::JsonPrintOptions opts;
    opts.add_whitespace = true;
    opts.always_print_primitive_fields = false;
    opts.preserve_proto_field_names = true;
    return opts;
};


class Descriptor
{
public:
    Descriptor();
    
    Descriptor(std::vector<std::string> proto_files, const Connection* connection);
    ~Descriptor();

    /**
     Create default message for given type name.
     */
    std::shared_ptr<google::protobuf::Message> create_message(const std::string& message_type_name);
    
    /**
     */
    std::string message_to_json(const google::protobuf::Message& message, google::protobuf::util::JsonPrintOptions jsonPrintOptions);
    
    /**
     */
    inline std::string message_to_json(const google::protobuf::Message& message) {
        return message_to_json(message, default_json_options());
    }
    
    /**
     */
    std::shared_ptr<google::protobuf::Message> message_from_json(const std::string& message_type_name, const std::string& json);
    
    /**
     Returns a nullptr if the given method can't be found.
     */
    const google::protobuf::MethodDescriptor* get_method_descriptor(const std::string& package_service_method);
    
    /**
     Return these descriptors as JSON
     */
    std::string to_json(google::protobuf::util::JsonPrintOptions options);
    
    inline std::string to_json() {
        return to_json(default_json_options());
    }
    
    /**
     */
    bool load_json(const std::string& json);
    
    const AuxlGRPCErrorCollector* get_error_collector();

private:
    std::shared_ptr<google::protobuf::Message> build_message(const google::protobuf::Descriptor& descriptor,
                                                             google::protobuf::DynamicMessageFactory& factory, int depth = 0, int max_depth = 1);
    
    AuxlGRPCErrorCollector* error_collector_;
    
    std::unique_ptr<google::protobuf::SimpleDescriptorDatabase> db_;
    std::unique_ptr<google::protobuf::DescriptorPool> pool_;
    std::unique_ptr<google::protobuf::DynamicMessageFactory> factory_;
};

} // ns grpc
} // ns auxl




#endif /* descriptor_hpp */
