
#include <iostream>
#include <string>
#include <vector>

#include "parser.h"
#include "types.h"
#include "connection.h"

/**
 */
int main(int argc, char **argv) {

    GRPCConfig config;

    config.endpoint = (char*) "localhost:4770";


    auto connection = auxl::grpc::create_connection(config);

    std::cout << "Connection state: " << connection->channel->GetState(true);


    // std::cout << "Hello world";
    
    // std::vector<std::string> vec;

    // vec.push_back("/Users/joostverrijt/Projects/var/auxl-grpc-mock/demo.proto");
    // vec.push_back("/Users/joostverrijt/Projects/var/auxl-grpc-mock/joost.proto");

    // try {
    //     auxl::grpc::proto_files_to_fd_json(vec);
    // } catch (const auxl::grpc::AuxlGRPCParserException& e) {
    //     std::cout << "Got error: " + std::string(e.message) + "\n";
    // }


    return 0;
}