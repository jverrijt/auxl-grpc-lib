
#include <iostream>
#include <string>
#include <vector>

#include "parser.h"
#include "util.h"

#include "descriptor.hpp"

#include "options.h"
#include "connection.h"
#include "error_collector.h"

#include <nlohmann/json.hpp>

#include <cxxopts.hpp>

using namespace auxl::grpc;

/**
 */
int cmd_describe(int argc, char **argv)
{
    cxxopts::Options options("auxl_grpc_cli describe", "Returns the descriptors of a given GRPC service and/or set of protofiles");
    options.add_options("describe")
        ("proto_files", "Comma delimited list of proto files", cxxopts::value<std::vector<std::string>>())
        ("endpoint", "The endpoint of GRPC service", cxxopts::value<std::string>())
        ("connection_config", "Path to a json file containing the connection options", cxxopts::value<std::string>());
    
    auto result = options.parse(argc, argv);
    
    if (result.arguments().size() == 0) {
        std::cout << options.help() << std::endl;
        return 0;
    }
    
    std::string endpoint, connection_config;
    std::vector<std::string> proto_files;
    
    if (result.count("endpoint")) {
        endpoint = result["endpoint"].as<std::string>();
    }
    
    if (result.count("proto_files")) {
        proto_files = result["proto_files"].as<std::vector<std::string>>();
    }
    
    GRPCConnectionOptions *opts = connection_options_default();
    if (result.count("connection_config")) {
        std::string connection_opts_json;
        
        if (util::load_file(result["connection_config"].as<std::string>(), &connection_opts_json)
            && !connection_opts_json.empty()) {
               
            connection_options_free(opts);
            opts = util::options_from_json(connection_opts_json);
        }
    }
    
    auto connection = Connection::create_connection(endpoint, *opts);
    connection_options_free(opts);
    
    Descriptor descriptor(proto_files, connection.get());
    
    std::string json_out = descriptor.to_json();
    
    auto error_collector = descriptor.get_error_collector();
     
    // Process errors
    if (error_collector->error_count > 0) {
        bool is_fatal = error_collector_has_fatal_error(error_collector);
        
        std::cerr << "Describing service " << (is_fatal ? "failed" : "finished")
            << "with the following issues:" << std::endl;
        
        for (int i = 0; i < error_collector->error_count; i++) {
            auto err = error_collector->errors[i];
            std::cerr << (err->level == FATAL ? "⛔️" : "⚠️") << ": " << err->message << std::endl;
        }
        
        if (is_fatal) {
            return 0;
        }
    }
    
    // Parse it so we can prettify.
    auto parsed_json = nlohmann::json::parse(json_out);
    
    std::cout << parsed_json.dump(4) << std::endl;
    
    return 0;
}

/**
 */
int cmd_message(int argc, char **argv)
{
    cxxopts::Options options("auxl_grpc_cli message", "Create messages with default values");
    options.add_options("message")
        ("type_name", "(required) The message type <package.message>", cxxopts::value<std::string>())
        ("descriptors", "(required) Path to the json representation descriptor", cxxopts::value<std::string>());
    
    auto result = options.parse(argc, argv);
    
    if (result.arguments().size() == 0 || !result.count("descriptors") || !result.count("type_name")) {
        std::cerr << options.help() << std::endl;
        return 1;
    }
    
    Descriptor descriptors;
    
    std::string descriptor_string;
    if (util::load_file(result["descriptors"].as<std::string>(), &descriptor_string) && !descriptor_string.empty()) {
        if (descriptors.load_json(descriptor_string) == false) {
            std::cerr << "Invalid descriptors" << std::endl;
            return 0;
        }
    } else {
        std::cerr << "Could not load descriptors at path: " << result["descriptors"].as<std::string>() << std::endl;
        return 0 ;
    }
    
    auto message = descriptors.create_message(result["type_name"].as<std::string>());
    
    if (message.get() == NULL) {
        std::cerr << "Message with given type not found" << std::endl;
        return 0;
    }
    
    google::protobuf::util::JsonPrintOptions json_opts;
    
    // This will ensure we se all the fields for a given message.
    json_opts.always_print_primitive_fields = true;
    
    
    auto json = descriptors.message_to_json(*message, json_opts);
    auto parsed_json = nlohmann::json::parse(json);
    
    std::cout << parsed_json.dump(4) << std::endl;
    
    return 0;
}

/**
 */
int main(int argc, char **argv)
{
//    char* new_args[5] = { (char*) "message", (char*) "--type_name", (char*) "greet.HelloRequest", (char*) "--descriptors", (char*) "/Users/joostverrijt/Projects/Metamotifs/vlui/auxl-grpc/test/test_resources/descriptor_local.json" };
//    int new_arg_c = 5;
//
//    return cmd_message(new_arg_c, new_args);
//
    
    if (argc == 1) {
        // Print usage
        std::cout << "No command given." << std::endl;
        std::cout << "Available commands are: describe, template, convert, and call." << std::endl;
        std::cout << "Use auxl_grpc_cli <command> to get more help on a specific command" << std::endl;
        return 0;
    }
    
    char* command = argv[1];
    
    if (strcmp(command, "describe") == 0) {
        cmd_describe(argc, argv);
    }
    else if (strcmp(command, "message") == 0) {
        cmd_message(argc, argv);
    }
    else {
        std::cout << "Unknown commmand:" << command;
        std::cout << "Available commands are: describe, message, and call." << std::endl;
    }

    return 0;
}
