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

std::unique_ptr<Connection> get_connection(std::string endpoint) {
    
    GRPCConnectionOptions options;
    options.use_ssl = false;
    options.ssl_root_certs_path = (char*) "/Users/joostverrijt/Projects/var/temp/roots.pem";
    
    auto connection = auxl::grpc::create_connection(endpoint, options);

    return connection;
}


std::shared_ptr<Descriptor> get_descriptor(std::vector<std::string> proto, std::string endpoint) {
    auto connection = get_connection(endpoint);
    auto d = Descriptor::create_descriptor(proto, connection.get());

    return d;
}

/**
 */
TEST_F(AuxlGrpcTest, ReflectServiceTest) {
    std::vector<std::string> proto;
    auto descr = get_descriptor(proto, "localhost:5000");
    
    google::protobuf::util::JsonPrintOptions options;
    
    std::string json = descr->to_json(options);
    
    std::cout << json << std::endl;
    
    EXPECT_TRUE(!json.empty());
}

/**
 */
TEST_F(AuxlGrpcTest, CreateMessage) {
    std::vector<std::string> proto;
    auto descr = get_descriptor(proto, "localhost:5000");
    
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

    auto descriptor = get_descriptor({}, "localhost:5000");

    Session sess(std::move(connection));
    
    auto message = descriptor->create_message("greet.HelloRequest");
    
    auto json = descriptor->message_to_json(*message);
    
    auto p = nlohmann::json::parse(json);
    p["name"] = "Request by Joost";
    
    auto message_altered = descriptor->message_from_json("greet.HelloRequest", p.dump());
    
    auto method_descr = descriptor->get_method_descriptor("greet.Greeter", "SayHello");
    
    
    
    sess.send_message(*message_altered, *method_descr);
    sess.close();
    
    // descriptor->message_to_json(*message, )
    
    
    
//
//    Session::create(*connection, "greet.Greeter", "SayHello", metadata, descriptor);
//    sess->delegate = std::shared_ptr<SessionDelegate>(new TestSessionDelegate());
//
//    // Create a message with some default values
//    auto tpl = auxl::grpc::create_template_message("greet.HelloRequest", descriptors);
//
//    auto p = nlohmann::json::parse(tpl);
//    p["name"] = "myRequest";
//    // p["handle"] = "xxx";
//
//    // Convert into something we can send over the wire.
//    auto msg = auxl::grpc::serialized_message_from_template("greet.HelloRequest", p.dump(), descriptors);
//
//    sess->start();
//    sess->send_message(msg);
//    sess->close();
}

///**
// */
//TEST_F(AuxlGrpcTest, SendStreaming)
//{
//    auto connection = get_connection("localhost:5000");
//    std::multimap<std::string, std::string> metadata;
//
//    auto descriptors = get_descriptor({}, "localhost:5000");
//
//    auto sess = Session::create(*connection, "greet.Greeter", "SayHelloServerStream", metadata, descriptors, 1);
//    sess->delegate = std::shared_ptr<SessionDelegate>(new TestSessionDelegate());
//
//    ASSERT_TRUE(sess->is_streaming());
//
//    // Generate some test message to send.
//    std::vector<std::string> tpls;
//    for (int i = 0; i < 10; i++) {
//        auto tpl = auxl::grpc::create_template_message("greet.HelloRequest", descriptors);
//        tpls.push_back(tpl);
//    }
//
//    sess->start();
//
//    std::string available_messages_prompt = "Select the message to send: 1-10, or q to end.\n";
//    std::cout << available_messages_prompt;
//
//    for (std::string line; std::getline(std::cin, line);) {
//        std::cout << line << std::endl;
//
//        if (line == "q") {
//            break;
//        }
//
//        int selection = atoi(line.c_str());
//
//        if (selection < 1 || selection > 10) {
//            std::cout << available_messages_prompt;
//        } else {
//            std::string tpl_to_send = tpls[selection];
//
//            auto p = nlohmann::json::parse(tpl_to_send);
//            p["name"] = "myRequest";
//            // p["handle"] = "xxx";
//
//            // std::cout << p.dump() << std::endl;
//
//            auto msg = auxl::grpc::serialized_message_from_template("greet.HelloRequest", p.dump(), descriptors);
//
//            sess->send_message(msg);
//
//            std::cout << available_messages_prompt;
//        }
//
//    }
//
//    sess->close();
//}


}  // namespace
}  // namespace project
}  // namespace my
