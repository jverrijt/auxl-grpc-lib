//
//  auxl_client_test.cpp
//  auxl-grpc
//
//  Created by Joost Verrijt on 03/01/2022.
//

#include <stdio.h>

#include "gtest/gtest.h"
#include "descriptor.hpp"
#include "connection.hpp"
#include "session.hpp"
#include "descriptor_session_delegate.hpp"
#include <nlohmann/json.hpp>
#include "options.h"

#include "cli_session_delegate.hpp"

#include "tool.h"


namespace auxl {
namespace grpc {
namespace {

class AuxlClientTest : public ::testing::Test {
};

/**
 Create a test message
 */
std::shared_ptr<google::protobuf::Message> create_input_message(Descriptor& descriptor, std::string type_name, std::string name)
{
    std::cout << "Creating message with type: " << type_name << std::endl;
    auto msg = descriptor.create_message(type_name);

    // Alter some values
    auto json = descriptor.message_to_json(*msg);
    auto p = nlohmann::json::parse(json);
    p["name"] = name;

    msg = descriptor.message_from_json(type_name, p.dump());
    
    return msg;
}

/**
 The example used in the README
 */
TEST_F(AuxlClientTest, TestExample)
{
    GRPCConnectionOptions options = init_connection_options();
    auto connection = Connection::create_connection("localhost:5000", options);
    
    // Create descriptors from server reflection
    Descriptor descriptor({}, connection.get());
    
    // Create a message
    auto message = descriptor.create_message("greet.HelloRequest");
    
    // The method that we will call
    const auto method_descr = descriptor.get_method_descriptor("greet.Greeter.SayHello");
    
    if (method_descr != nullptr && message != nullptr)
    {
        Session session(connection.get());
        
        // A session delegates allows for capturing and formatting output.
        // It also captures any errors and other session events.
        DescriptorSessionDelegate delegate(&descriptor, "HelloResponse");
        session.delegate = &delegate;
        
        session.start(*method_descr);
        session.send_message(*message);
        session.close();
    }
}

/**
 */
TEST_F(AuxlClientTest, TestDescriptorClient)
{
    std::cout << "descriptor test" << std::endl;
    
    GRPCConnectionOptions options = init_connection_options();
    auto connection = Connection::create_connection("localhost:5000", options);
    
    Descriptor descriptor({}, connection.get());
    const auto method_descr = descriptor.get_method_descriptor("greet.Greeter.SayHello");
    
    ASSERT_TRUE(method_descr != nullptr);
    
    DescriptorSessionDelegate delegate(&descriptor, method_descr->output_type()->full_name());

    Session session(connection.get());
    session.delegate = &delegate;
    
    auto message = create_input_message(descriptor, method_descr->input_type()->full_name(), "Test Message");
    
    session.start(*method_descr);
    session.send_message(*message);
    session.close();
}

/**
 */
TEST_F(AuxlClientTest, TestMessageGeneration)
{
    Descriptor descriptor({"test_resources/greet.proto"}, NULL);
    
    // Does not exist
    auto msg = descriptor.create_message("test");
    ASSERT_TRUE(msg.get() == nullptr);
    
    msg = descriptor.create_message("greet.HelloRequest");
    ASSERT_TRUE(msg.get() != nullptr);
    
    std::string test;
    util::load_file("test_resources/greet.proto", &test);
    
    ASSERT_FALSE(test.empty());
}

/**
 */
TEST_F(AuxlClientTest, TestParseConnectionConfig)
{
    std::string json_config;
    util::load_file("test_resources/connection_opts_test.json", &json_config);

    ASSERT_FALSE(json_config.empty());
    
    auto opts = util::options_from_json(json_config);
    
    ASSERT_TRUE(opts->use_ssl);
    ASSERT_TRUE(strcmp(opts->ssl_root_certs_path, (char*) "Path to roots.pem") == 0);
    
    // connection_options_free(opts);
}

/**
 */
TEST_F(AuxlClientTest, TestParseJsonDescriptor)
{
    Descriptor descriptor({"test_resources/greet.proto"}, NULL);
    
    auto json = descriptor.to_json();
    
    std::cout << json << std::endl;
    
    ASSERT_FALSE(json.empty());
    
    Descriptor d;
    
    d.load_json(json);
    
    ASSERT_FALSE(d.to_json().empty());
}


}
} // ns grpc
} // ns auxl
