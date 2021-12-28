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

namespace auxl {
namespace grpc {
namespace {

// The fixture for testing class Foo.
class AuxlGrpcTest : public ::testing::Test {
};

// Tests that the Foo::Bar() method does Abc.
TEST_F(AuxlGrpcTest, ReflectServiceTest) {

    GRPCConfig config;
    config.options.use_ssl = false;
    config.options.ssl_root_certs_path = (char*) "/Users/joostverrijt/Projects/var/temp/roots.pem";
    
    config.endpoint = (char*) "localhost:4770";

    auto connection = auxl::grpc::create_connection(config);

    std::vector<std::string> proto_files;
    std::string json = auxl::grpc::describe(proto_files, &connection);

    std::cout << json;
    
  // EXPECT_EQ(f.Bar(input_filepath, output_filepath), 0);
}



}  // namespace
}  // namespace project
}  // namespace my
