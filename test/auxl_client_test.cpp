//
//  auxl_client_test.cpp
//  auxl-grpc
//
//  Created by Joost Verrijt on 03/01/2022.
//

#include <stdio.h>

#include "gtest/gtest.h"
#include "descriptor.hpp"
#include "connection.h"
#include "session.h"
#include "descriptor_session_delegate.hpp"
#include <nlohmann/json.hpp>


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
 */
TEST_F(AuxlClientTest, TestDescriptorClient)
{
    std::cout << "descriptor test" << std::endl;
    
    GRPCConnectionOptions options;
    options.use_ssl = false;
    auto connection = Connection::create_connection("localhost:5000", options);
    
    Descriptor descriptor({}, connection.get());
    
    const auto method_descr = descriptor.get_method_descriptor("greet.Greeter", "SayHello");
    
    DescriptorSessionDelegate delegate(&descriptor, method_descr->output_type()->full_name());

    Session session(connection.get());
    session.delegate = &delegate;
    
    auto message = create_input_message(descriptor, method_descr->input_type()->full_name(), "Test Message");
    
    session.start(*method_descr);
    session.send_message(*message);
    session.close();
}

}
} // ns grpc
} // ns auxl
