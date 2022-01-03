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
#include "session.h"


#include "error_collector.h"

namespace auxl {
namespace grpc {
namespace {

class AuxlGrpcTest : public ::testing::Test {
};

class TestSessionDelegate: public SessionDelegate {
    
public:
    void did_receive(std::string response, std::multimap<::grpc::string_ref, ::grpc::string_ref> meta_data) override {
        std::cout << "Delegate received : " << response << std::endl;
    }

    void session_did_close(::grpc::Status stat, std::multimap<::grpc::string_ref, ::grpc::string_ref> metadata) override
    {
        std::cout << "Session did close with status: " <<  stat.ok() << std::endl;
    }
};

std::unique_ptr<Connection> get_connection(std::string endpoint)
{
    GRPCConnectionOptions options;
    options.use_ssl = false;
    options.ssl_root_certs_path = (char*) "/Users/joostverrijt/Projects/var/temp/roots.pem";
    
    auto connection = Connection::create_connection(endpoint, options);

    return connection;
}

/**
 */
TEST_F(AuxlGrpcTest, ErrorCollectorTest)
{
    auto col = create_error_collector();
    
    error_collector_add_error(col, PROTO_ERROR, (char*) "proto file could not be found", WARNING);
    error_collector_add_error(col, ENDPOINT_NOT_REACHEABLE, (char*) "endpoint not reachable", WARNING);
    ASSERT_TRUE(col->error_count == 2);
    
    for (int i = 0; i < col->error_count; i++) {
         printf("Got error: %s - %d", col->errors[i]->message, col->errors[i]->type);
    }
    
    auto new_col = error_collector_copy(col);
    
    error_collector_free(col);
    
    ASSERT_TRUE(new_col->error_count == 2);
    
}

/**
 */
TEST_F(AuxlGrpcTest, DescriptorFromProtoTest)
{
    std::vector<std::string> proto = { "test_resources/greet.proto" };
    auto descr = std::unique_ptr<Descriptor>(new Descriptor(proto, nullptr));
    std::cout << descr->to_json() << std::endl;
    
    ASSERT_TRUE(descr->get_error_collector()->error_count == 0);
    
    // Load an inexistent proto
    
    std::vector<std::string> bogus_proto = { "test_resources/does_not_exist.proto" };
    auto descr_b = std::unique_ptr<Descriptor>(new Descriptor(bogus_proto, nullptr));
    
    auto col = descr_b->get_error_collector();
    
    ASSERT_TRUE(col->error_count == 1);
    ASSERT_TRUE(col->errors[0]->type == PROTO_ERROR);
    
    for (int i = 0; i < col->error_count; i++) {
         printf("Got error: %s - %d", col->errors[i]->message, col->errors[i]->type);
    }

}

/**
 */
TEST_F(AuxlGrpcTest, ReflectServiceTest) {
    std::vector<std::string> proto;
    auto descr = std::unique_ptr<Descriptor>(new Descriptor(proto, get_connection("localhost:5000").get()));
    
    google::protobuf::util::JsonPrintOptions options;
    
    std::string json = descr->to_json(options);
    
    std::cout << json << std::endl;
    
    EXPECT_TRUE(!json.empty());
}

/**
 */
TEST_F(AuxlGrpcTest, CreateMessage) {
    std::vector<std::string> proto;
    auto descr = std::unique_ptr<Descriptor>(new Descriptor(proto, get_connection("localhost:5000").get()));
    
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
    Descriptor descriptor({}, connection.get());
    
    Session sess(connection.get());
    
    TestSessionDelegate delegate;
    sess.delegate = &delegate;
    
    auto method_descr = descriptor.get_method_descriptor("greet.Greeter", "SayHello");
    std::string input_type_name = method_descr->input_type()->full_name();
    
    std::cout << "Creating input message: " << input_type_name << std::endl;
    auto message = descriptor.create_message(input_type_name);
    
    // Alter some values
    auto json = descriptor.message_to_json(*message);
    auto p = nlohmann::json::parse(json);
    p["name"] = "Request by Joost";
    
    message = descriptor.message_from_json(input_type_name, p.dump());
    
    sess.start(*method_descr);
    sess.send_message(*message);
    sess.close();
}

/**
 */
TEST_F(AuxlGrpcTest, TestClientStream)
{
    auto connection = get_connection("localhost:5000");
    std::multimap<std::string, std::string> metadata;

    auto descriptor = std::unique_ptr<Descriptor>(new Descriptor({}, connection.get()));
    
    Session session(connection.get());
    
    auto method_descr = descriptor->get_method_descriptor("greet.Greeter", "SayHelloBidiStream");

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
