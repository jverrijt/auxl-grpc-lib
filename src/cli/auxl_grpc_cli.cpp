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

#include <iostream>
#include <string>
#include <vector>
#include <tuple>

#include "parser.hpp"
#include "util.hpp"

#include "descriptor.hpp"

#include "options.h"
#include "connection.hpp"
#include "session.hpp"
#include "error_collector.h"
#include "cli_session_delegate.hpp"
#include "cli_util.hpp"

#include <nlohmann/json.hpp>

#include <cxxopts.hpp>

using namespace auxl::grpc;
using namespace auxl::grpc::cli;

/**
 */
int cmd_describe(int argc, char **argv)
{
    cxxopts::Options options("auxl_grpc_cli describe", "Returns the descriptors of a given GRPC service and/or set of protofiles");
    options.add_options("describe")
    ("proto_files", "Comma delimited list of proto files", cxxopts::value<std::vector<std::string>>())
    ("endpoint", "The endpoint of GRPC service", cxxopts::value<std::string>())
    ("connection_options", "Path to a json file containing the connection options", cxxopts::value<std::string>());
    
    cxxopts::ParseResult result;
    
    try {
        result = options.parse(argc, argv);
    } catch(const cxxopts::OptionException& e) {
        std::cout << e.what() << std::endl;
        return 0;
    }
    
    if (result.arguments().size() == 0) {
        std::cout << options.help() << std::endl;
        return 0;
    }
    
    std::string endpoint, connection_config;
    std::vector<std::string> proto_files;
    
    std::shared_ptr<Connection> connection;
    
    if (result.count("endpoint")) {
        auto connection_options_path = result.count("connection_options") ?
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
    
    cxxopts::ParseResult result;
    
    try {
        result = options.parse(argc, argv);
    } catch(const cxxopts::OptionException& e) {
        std::cout << e.what() << std::endl;
        return 0;
    }
    
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
    ("connection_options", "Path to a json file containing the connection options", cxxopts::value<std::string>())
    ("metadata", "Pass along metadata. Format: key:val,key:val,key:val,...", cxxopts::value<std::vector<std::string>>());
    
    cxxopts::ParseResult result;
    
    try {
        result = options.parse(argc, argv);
    } catch(const cxxopts::OptionException& e) {
        std::cout << e.what() << std::endl;
        return 0;
    }
    
    if (result.arguments().size() == 0) {
        std::cout << options.help() << std::endl;
        return 0;
    }
    
    if (!result.count("endpoint") || !result.count("method") || !result.count("descriptors")) {
        std::cout << "Missing required option" << std::endl;
        std::cout << options.help() << std::endl;
        return 0;
    }
    
    std::multimap<std::string, std::string> metadata;
    if (result.count("metadata")) {
        auxl::grpc::cli::parse_metadata(result["metadata"].as<std::vector<std::string>>(), metadata);
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
    
    auto connection_options_path = result.count("connection_options") ?
    &(result["connection_options"].as<std::string>()) : NULL;
    
    auto connection = cli_create_connection(result["endpoint"].as<std::string>(), connection_options_path);
    
    std::vector<std::string> message_files = result["message"].as<std::vector<std::string>>();
    
    if (message_files.empty()) {
        std::cerr << "No message to send" << std::endl;
        return 0;
    }
    
    std::string service_and_method = result["method"].as<std::string>();
    const auto method_descr = descriptors.get_method_descriptor(service_and_method);
    
    GRPCCallInfo call_info = util::call_info(*method_descr);
    
    std::cout << "Starting a " << call_info.name << " call to " << service_and_method << std::endl;
    
    Session session(connection.get());
    
    CliSessionDelegate session_delegate(&descriptors, method_descr->output_type()->full_name(), call_info.type);
    session.delegate = &session_delegate;
    
    if(!session.start(*method_descr, metadata)) {
        std::cerr << "[ERR] Service unreachable" << std::endl;
        return 0;
    }
    
    if (message_files.size() > 1) {
        auto print_available_message_prompt = [](std::vector<std::string> message_files) {
            std::cout << "Available messages: \n";
            for (int i = 0; i < message_files.size(); i++) {
                std::cout << i + 1 << ". " << message_files[i] << std::endl;
            }
            std::cout << "Enter # of message to send or q to quit." << std::endl;
        };
        
        print_available_message_prompt(message_files);
        
        for (std::string line; std::getline(std::cin, line);) {
            if (line == "q") {
                break;
            }
            
            int selection = atoi(line.c_str());
            
            if (selection < 1 || selection > message_files.size()) {
                print_available_message_prompt(message_files);
            } else {
                load_and_send_message_at_path(message_files[selection - 1], method_descr->input_type()->full_name(), descriptors, session);
                
                // Here the user has provided multiple messages to a unary or server streaming call so break after the selected message is sent.
                if (call_info.type == Unary || call_info.type == ServerStreaming) {
                    break;
                }
            }
        }
    } else if (message_files.size() == 1) {
        load_and_send_message_at_path(message_files[0], method_descr->input_type()->full_name(), descriptors, session);
    }
    
    session.close();
    
    return 0;
}

/* For test purposes, remove */
int test_call()
{
    char* new_args[13] = {
        (char*) "call",
        (char*) "--endpoint",
        (char*) "localhost:5000",
        (char*) "--method",
        (char*) "greet.Greeter.SayHelloBidiStream",
        (char*) "--descriptors",
        (char*) "test_resources/descriptor_local.json",
        (char*) "--message",
        (char*) "test_resources/hello_request_message_1.json",
        (char*) "--message",
        (char*) "test_resources/hello_request_message_2.json",
        (char*) "--metadata",
        (char*) "key_a:val_a,key_b:val_b"
        
    };
    int new_arg_c = 13;
    
    return cmd_call(new_arg_c, new_args);
}

/* For test purposes, remove */
int test_describe()
{
    char* new_args[5] = {
        (char*) "describe",
        (char*) "--endpoint",
        (char*) "demo.gripgrpc.dev:443",
        (char*) "--connection_options",
        (char*) "test_resources/connection_config.json",
    };
    int new_arg_c = 5;
    
    return cmd_describe(new_arg_c, new_args);
    
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
