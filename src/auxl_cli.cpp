
#include <iostream>
#include <string>
#include <vector>
#include <tuple>

#include "parser.h"
#include "util.h"

#include "descriptor.hpp"

#include "options.h"
#include "connection.h"
#include "session.h"
#include "error_collector.h"

#include <nlohmann/json.hpp>

#include <cxxopts.hpp>

using namespace auxl::grpc;

/**
 Create connection with loading options from a given path.
 */
std::shared_ptr<Connection> cli_create_connection(const std::string& endpoint, const std::string* path_to_options)
{
    std::shared_ptr<Connection> connection;
    GRPCConnectionOptions *opts = connection_options_default();

    if (path_to_options != NULL) {
        std::string connection_opts_json;
        
        if (util::load_file(*path_to_options, &connection_opts_json)
            && !connection_opts_json.empty()) {
               
            connection_options_free(opts);
            opts = util::options_from_json(connection_opts_json);
        }
    }
    
    auto conn = Connection::create_connection(endpoint, *opts);

    connection = std::move(conn);
    connection_options_free(opts);

    return connection;
}

/**
 Returns the type of the method
 */
std::string rpc_method_type(const google::protobuf::MethodDescriptor& descr)
{
    if (descr.client_streaming() && descr.server_streaming()) {
        return "bidirectional streaming";
    } else if (descr.client_streaming() && !descr.server_streaming()) {
        return "client streaming";
    } else if (!descr.client_streaming() && descr.server_streaming()) {
        return "server streaming";
    } else {
        return "unary";
    }
}

/**
 */
void load_and_send_message_at_path(const std::string& message_path,
                                     const std::string& message_type_name,
                                     Descriptor& descriptors,
                                     Session& session)
{
    std::string json_message;
    if (util::load_file(message_path, &json_message)) {
        auto message = descriptors.message_from_json(message_type_name, json_message);
        session.send_message(*message);
    } else {
        std::cerr << "Could not load message at " << message_path << std::endl;
    }
}

/**
 */
int cmd_describe(int argc, char **argv)
{
    cxxopts::Options options("auxl_grpc_cli describe", "Returns the descriptors of a given GRPC service and/or set of protofiles");
    options.add_options("describe")
        ("proto_files", "Comma delimited list of proto files", cxxopts::value<std::vector<std::string>>())
        ("endpoint", "The endpoint of GRPC service", cxxopts::value<std::string>())
        ("connection_options", "Path to a json file containing the connection options", cxxopts::value<std::string>());
    
    auto result = options.parse(argc, argv);
    
    if (result.arguments().size() == 0) {
        std::cout << options.help() << std::endl;
        return 0;
    }
    
    std::string endpoint, connection_config;
    std::vector<std::string> proto_files;
    
    std::shared_ptr<Connection> connection;
    
    if (result.count("endpoint")) {
        auto connection_options_path = result.count("connection_config") ?
            &(result["connection_options"].as<std::string>()) : NULL;
        connection = cli_create_connection(result["endpoint"].as<std::string>(), connection_options_path);
    }
    
    if (result.count("proto_files")) {
        proto_files = result["proto_files"].as<std::vector<std::string>>();
    }
    
    Descriptor descriptor(proto_files, connection.get());
    
    std::string json_out = descriptor.to_json();
    
    auto error_collector = descriptor.get_error_collector();
     
    // Process errors
    if (error_collector->error_count > 0) {
        bool is_fatal = error_collector_has_fatal_error(error_collector);
        
        std::cerr << "Describing service " << (is_fatal ? "failed" : "finished")
            << " with the following issues:" << std::endl;
        
        for (int i = 0; i < error_collector->error_count; i++) {
            auto err = error_collector->errors[i];
            std::cerr << (err->level == FATAL ? "[ERR]" : "[WARN]") << ": " << err->message << std::endl;
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
 GRPC call service and method.
 */
int cmd_call(int argc, char** argv)
{
    cxxopts::Options options("auxl_grpc_cli call", "Call GRPC services");
    options.add_options("call")
        ("endpoint", "(required) The endpoint of GRPC service", cxxopts::value<std::string>())
        ("method", "(required) The method which to call. Formulate it like so: package.service.method", cxxopts::value<std::string>())
        ("descriptors", "(required) Path to the json representation descriptor", cxxopts::value<std::string>())
        ("message", "Path to a message to send. Repeat this option to allow sending multiple messages (i.e. streaming)", cxxopts::value<std::vector<std::string>>())
        ("connection_options", "Path to a json file containing the connection options", cxxopts::value<std::string>());
    
    auto result = options.parse(argc, argv);
    
    if (result.arguments().size() == 0) {
        std::cout << options.help() << std::endl;
        return 0;
    }
    
    if (!result.count("endpoint") || !result.count("method") || !result.count("descriptors")) {
        std::cout << "Missing required option" << std::endl;
        std::cout << options.help() << std::endl;
        return 0;
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
    
    auto connection_options_path = result.count("connection_config") ?
        &(result["connection_options"].as<std::string>()) : NULL;
    
    auto connection = cli_create_connection(result["endpoint"].as<std::string>(), connection_options_path);
    
    auto service_and_method = util::split_service_method(result["method"].as<std::string>());
    
    if (std::get<0>(service_and_method) == false) {
        std::cerr << "Invalid service / method name" << std::endl;
        std::cout << options.help() << std::endl;
    }
    
    const auto method_descr = descriptors.get_method_descriptor(std::get<1>(service_and_method), std::get<2>(service_and_method));
    
    std::cout << "Starting a " << rpc_method_type(*method_descr) << " call to " << std::get<1>(service_and_method) << "." << std::get<2>(service_and_method) << std::endl;
    
    Session session(connection.get());
    session.start(*method_descr);

    std::vector<std::string> message_files = result["message"].as<std::vector<std::string>>();
    
    if (message_files.size() > 1) {
        auto print_available_message_prompt = [](std::vector<std::string> message_files) {
            std::cout << "Select the message to send or q to end.\n";
            for (int i = 0; i < message_files.size(); i++) {
                std::cout << i + 1 << ". " << message_files[i];
            }
        };

        for (std::string line; std::getline(std::cin, line);) {
            std::cout << line << std::endl;

            if (line == "q") {
                break;
            }

            int selection = atoi(line.c_str());

            if (selection < 1 || selection > message_files.size()) {
                print_available_message_prompt(message_files);
            } else {
                load_and_send_message_at_path(message_files[selection], method_descr->input_type()->full_name(), descriptors, session);
                print_available_message_prompt(message_files);
            }
        }
    } else if (message_files.size() == 1) {
        load_and_send_message_at_path(message_files[0], method_descr->input_type()->full_name(), descriptors, session);
    } else {
        // Empty message? test this?
    }

    session.close();
    
    return 0;
}


void print_usage() {
    std::cout << "Usage: auxl_grpc_cli <command> [OPTIONS...]\n" << std::endl;
    std::cout << "Available commands are:\n\tdescribe\n\tmessage\n\tcall\n" << std::endl;
    std::cout << "Use auxl_grpc_cli <command> to get more help on a specific command\n" << std::endl;
}

/**
 */
int main(int argc, char **argv)
{
//    char* new_args[7] = {
//        (char*) "call",
//        (char*) "--endpoint",
//        (char*) "localhost:5000",
//        (char*) "--method",
//        (char*) "greet.Greeter.SayHello",
//        (char*) "--message",
//        (char*) "x.json"
//    };
//    int new_arg_c = 7;
//
//    return cmd_call(new_arg_c, new_args);
    
    
    // bin/auxl_grpc_cli describe --proto_files /Users/joostverrijt/Projects/var/auxl-grpc-mock/demo.proto
    
//        char* new_args[5] = { (char*) "describe", (char*) "--proto_files", (char*) "/Users/joostverrijt/Projects/var/auxl-grpc-mock/demo.proto" };
//        int new_arg_c = 3;
//
//        return cmd_describe(new_arg_c, new_args);
    
//    char* new_args[5] = { (char*) "describe", (char*) "--endpoint", (char*) "localhost:323" };
//    int new_arg_c = 3;
//
//    return cmd_describe(new_arg_c, new_args);
    
//    char* new_args[5] = { (char*) "message", (char*) "--type_name", (char*) "greet.HelloRequest", (char*) "--descriptors", (char*) "/Users/joostverrijt/Projects/Metamotifs/vlui/auxl-grpc/test/test_resources/descriptor_local.json" };
//    int new_arg_c = 5;
//
//    return cmd_message(new_arg_c, new_args);
//
    
    if (argc == 1) {
        // Print usage
        print_usage();
        return 0;
    }
    
    char* command = argv[1];
    
    if (strcmp(command, "describe") == 0) {
        cmd_describe(argc, argv);
    }
    else if (strcmp(command, "message") == 0) {
        cmd_message(argc, argv);
    }
    else if (strcmp(command, "call") == 0) {
        cmd_call(argc, argv);
    }
    
    else {
        std::cout << "Unknown commmand." << std::endl;
        print_usage();
    }

    return 0;
}
