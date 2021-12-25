
#include <iostream>
#include <string>
#include <vector>

#include "parser.h"
#include "types.h"
#include "connection.h"

/**
 */
int main(int argc, char **argv) {

    std::vector<std::string> proto_files;
    proto_files.push_back("/Users/joostverrijt/Projects/var/auxl-grpc-mock/demo.proto");

    // auxl::grpc::describe(proto_files, NULL);
    // return 0;

    GRPCConfig config;
    config.options.use_ssl = false;
    config.options.ssl_root_certs_path = (char*) "/Users/joostverrijt/Projects/var/temp/roots.pem";

    // config.endpoint = (char*) "demo.gripgrpc.dev:443";
    config.endpoint = (char*) "localhost:4770";

    auto connection = auxl::grpc::create_connection(config);
    std::cout << "Connection state: " << connection->channel->GetState(true) << std::endl;

    // auxl::grpc::proto_files_to_fd_json()

    // std::vector<std::string> protos;
    
    auxl::grpc::describe(proto_files, &connection);

    return 0;
}