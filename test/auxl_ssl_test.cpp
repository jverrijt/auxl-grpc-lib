//
//  auxl_ssl_test.cpp
//  auxl_grpc_cli
//
//  Created by Joost Verrijt on 18/07/2022.
//

#include <stdio.h>

#include <nlohmann/json.hpp>

#include "auxl_grpc.h"
#include "gtest/gtest.h"

#include "descriptor.hpp"
#include "connection.hpp"
#include "session.hpp"
#include "descriptor_session_delegate.hpp"
#include "error_collector.h"
#include "test_defines.h"

#define GRPC_ROOT_PEM_PATH "/Users/joostverrijt/Projects/Metamotifs/libvlui/vlui/Resources/cert/cacert.pem"

AUXLGRPC_NS_BEGIN

class AuxlGrpcSSLTest : public ::testing::Test {
};

/**
 */
std::unique_ptr<Connection> get_connection(const std::string& endpoint)
{
    GRPCConnectionOptions *options = connection_options_default();
    options->timeout = 100;
    options->enable_tls = true;
    options->validate_tls = false;
    
    std::string cert_path = "/Users/joostverrijt/Projects/Metamotifs/libvlui/vlui/Resources/cert/cacert.pem";
    options->pem_root_certs_path = const_cast<char*>(cert_path.c_str());
    
    auto connection = Connection::create_connection(endpoint, *options);

    return connection;
}

/**
 */
TEST_F(AuxlGrpcSSLTest, TestServiceReflect)
{
    auto connection = get_connection(TEST_SSL_ENDPOINT);

    std::vector<std::string> proto;
    auto descr = std::unique_ptr<Descriptor>(new Descriptor(proto, get_connection(TEST_SSL_ENDPOINT).get()));

    google::protobuf::util::JsonPrintOptions options;

    std::string json = descr->to_json(options);
    std::cout << json << std::endl;

    auto error_collector = descr->get_error_collector();
    EXPECT_TRUE(error_collector->error_count == 0);
    
    EXPECT_TRUE(!json.empty());
}

AUXLGRPC_NS_END
