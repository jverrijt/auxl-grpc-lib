//
//  auxl_main_test.cpp
//  auxl_grpc_cli
//
//  Created by Joost Verrijt on 28/12/2021.
//

#include <stdio.h>
#include "gtest/gtest.h"

#include "connection.h"
#include "parser.h"
#include "template.h"

namespace auxl {
namespace grpc {
namespace {

// The fixture for testing class Foo.
class AuxlGrpcTest : public ::testing::Test {
};

std::string get_descriptor(std::vector<std::string> proto, std::string endpoint) {
    GRPCConfig config;
    config.options.use_ssl = false;
    config.options.ssl_root_certs_path = (char*) "/Users/joostverrijt/Projects/var/temp/roots.pem";
    
    config.endpoint = (char*) endpoint.c_str();

    auto connection = auxl::grpc::create_connection(config);

    std::vector<std::string> proto_files;
    std::string json = auxl::grpc::describe(proto_files, &connection);

    return json;
}

/**
 */
TEST_F(AuxlGrpcTest, ReflectServiceTest) {
    std::vector<std::string> proto;
    auto descr = get_descriptor(proto, "localhost:4770");
    
    EXPECT_TRUE(!descr.empty());
}

/**
 */
TEST_F(AuxlGrpcTest, CreateTemplate) {
    std::vector<std::string> proto;
    auto descr = get_descriptor(proto, "localhost:4770");
    
    auto tpl = auxl::grpc::create_template_message("demo.EchoMessage", descr);
    
    EXPECT_TRUE(!tpl.empty());
}



}  // namespace
}  // namespace project
}  // namespace my
