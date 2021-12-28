
#include <iostream>
#include <string>
#include <vector>

#include "parser.h"
#include "template.h"
#include "util.h"

#include "types.h"
#include "connection.h"

#include <nlohmann/json.hpp>

#include <cxxopts.hpp>


/**
 */
int cmd_describe(int argc, char **argv)
{
    cxxopts::Options options("auxl_grpc_cli describe", "Returns the descriptors of a given GRPC service and/or set of protofiles");
    options.add_options("describe")
        ("proto_files", "Comma delimited list of proto files", cxxopts::value<std::vector<std::string>>())
        ("endpoint", "The endpoint of GRPC service", cxxopts::value<std::string>());
    
    auto result = options.parse(argc, argv);
    
    if (result.arguments().size() == 0) {
        std::cout << options.help() << std::endl;
        return 0;
    }
    
    std::string endpoint;
    std::vector<std::string> proto_files;
    
    
    if (result.count("endpoint")) {
        endpoint = result["endpoint"].as<std::string>();
    }
    
    if (result.count("proto_files")) {
        proto_files = result["proto_files"].as<std::vector<std::string>>();
    }
    
    GRPCConfig config;
    config.options.use_ssl = false;
    config.options.ssl_root_certs_path = (char*) "/Users/joostverrijt/Projects/var/temp/roots.pem";
    
    config.endpoint = (char*) endpoint.c_str();

    auto connection = auxl::grpc::create_connection(config);

    std::string json = auxl::grpc::describe(proto_files, &connection);
    
    // Parse it so we can prettify.
    auto parsed_json = nlohmann::json::parse(json);
    
    std::cout << parsed_json.dump(4) << std::endl;
    
    return 0;
}

/**
 */
int cmd_template(int argc, char **argv)
{
    cxxopts::Options options("auxl_grpc_cli template", "Create template messages with default values");
    options.add_options("template")
        ("type", "The message type", cxxopts::value<std::string>())
        ("descriptor", "The descriptor", cxxopts::value<std::string>());
    
    auto result = options.parse(argc, argv);
    
    if (result.arguments().size() == 0) {
        std::cout << options.help() << std::endl;
        return 0;
    }

    std::string descriptor = auxl::grpc::util::load_file(result["descriptor"].as<std::string>());
    auxl::grpc::create_template_message(result["type"].as<std::string>(), descriptor);
    
    return 0;
}

/**
 */
int main(int argc, char **argv)
{
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
    else if (strcmp(command, "template") == 0) {
        cmd_template(argc, argv);
    }
    else {
        std::cout << "Unknown commmand:" << command;
        std::cout << "Available commands are: describe, template, convert, and call." << std::endl;
    }

    return 0;
}
