//
//  auxl_main_test.cpp
//  auxl_grpc_cli
//
//  Created by Joost Verrijt on 28/12/2021.
//

#include <stdio.h>

#include <nlohmann/json.hpp>

#include "gtest/gtest.h"

#include "descriptor.hpp"
#include "connection.h"
#include "parser.h"
#include "template.h"
#include "session.h"

namespace auxl {
namespace grpc {
namespace {

// The fixture for testing class Foo.
class AuxlGrpcTest : public ::testing::Test {
};


class TestSessionDelegate: public SessionDelegate {
    
public:
    void did_receive(std::string response, std::multimap<::grpc::string_ref, ::grpc::string_ref> meta_data) override {
        std::cout << "Delegate received : " << response << std::endl;
    }
};

std::unique_ptr<Connection> get_connection(std::string endpoint)
{
    GRPCConnectionOptions options;
    options.use_ssl = false;
    options.ssl_root_certs_path = (char*) "/Users/joostverrijt/Projects/var/temp/roots.pem";
    
    auto connection = auxl::grpc::create_connection(endpoint, options);

    return connection;
}

/**
 */
TEST_F(AuxlGrpcTest, ReflectServiceTest) {
    std::vector<std::string> proto;
    auto descr = Descriptor::create_descriptor(proto, get_connection("localhost:5000").get());
    
    google::protobuf::util::JsonPrintOptions options;
    
    std::string json = descr->to_json(options);
    
    std::cout << json << std::endl;
    
    EXPECT_TRUE(!json.empty());
}

/**
 */
TEST_F(AuxlGrpcTest, CreateMessage) {
    std::vector<std::string> proto;
    auto descr = Descriptor::create_descriptor(proto, get_connection("localhost:5000").get());
    
    auto not_existing_msg = descr->create_message("bogus.does_not_exist");
    ASSERT_TRUE(not_existing_msg.get() == nullptr);
    
    auto msg = descr->create_message("greet.HelloRequest");
    ASSERT_TRUE(msg.get() != nullptr);
    
    std::string msg_json = descr->message_to_json(*msg);
    
    std::cout << msg_json << std::endl;
    
}

/**
 Send a unary request
 */
TEST_F(AuxlGrpcTest, SendUnary) {
    auto connection = get_connection("localhost:5000");
    std::multimap<std::string, std::string> metadata;

    // Use server reflection
    auto descriptor = Descriptor::create_descriptor({}, connection.get());
    
    Session sess(std::move(connection));
    
    auto method_descr = descriptor->get_method_descriptor("greet.Greeter", "SayHello");
    
    std::string input_type_name = method_descr->input_type()->full_name();
    
    std::cout << "Creating input message: " << input_type_name << std::endl;
    auto message = descriptor->create_message(input_type_name);
    
    // Alter some values
    auto json = descriptor->message_to_json(*message);
    auto p = nlohmann::json::parse(json);
    p["name"] = "Request by Joost";
    
    message = descriptor->message_from_json(input_type_name, p.dump());
    
    sess.start(*method_descr);
    sess.send_message(*message);
    sess.close();
}

/**
 */
TEST_F(AuxlGrpcTest, TestServerStream)
{
    auto connection = get_connection("localhost:5000");
    std::multimap<std::string, std::string> metadata;

    auto descriptor = Descriptor::create_descriptor({}, connection.get());
    
    Session session(std::move(connection));
    
    auto method_descr = descriptor->get_method_descriptor("greet.Greeter", "SayHelloServerStream");

    // Generate some test message to send.
    std::vector<std::shared_ptr<google::protobuf::Message>> messages;
    for (int i = 0; i < 10; i++) {
        auto message = descriptor->create_message(method_descr->input_type()->full_name());
        
        auto json = descriptor->message_to_json(*message);
        auto p = nlohmann::json::parse(json);
        p["name"] = "Request #" + std::to_string(i);
        
        message = descriptor->message_from_json(method_descr->input_type()->full_name(), p.dump());
            
        messages.push_back(message);
    }

    session.start(*method_descr);

    std::string available_messages_prompt = "Select the message to send: 1-10, or q to end.\n";
    std::cout << available_messages_prompt;

    for (std::string line; std::getline(std::cin, line);) {
        std::cout << line << std::endl;

        if (line == "q") {
            break;
        }

        int selection = atoi(line.c_str());

        if (selection < 1 || selection > 10) {
            std::cout << available_messages_prompt;
        } else {
            std::shared_ptr<google::protobuf::Message> msg_to_send = messages[selection];
            session.send_message(*msg_to_send);

            std::cout << available_messages_prompt;
        }

    }

    session.close();
}


}  // namespace
}  // namespace project
}  // namespace my