//
//  auxl_main_test.cpp
//  auxl_grpc_cli
//
//  Created by Joost Verrijt on 28/12/2021.
//

#include <stdio.h>

#include <nlohmann/json.hpp>

#include "gtest/gtest.h"

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

std::shared_ptr<Connection> get_connection(std::string endpoint) {
    GRPCConfig config;
    config.options.use_ssl = false;
    config.options.ssl_root_certs_path = (char*) "/Users/joostverrijt/Projects/var/temp/roots.pem";
    
    config.endpoint = (char*) endpoint.c_str();

    auto connection = auxl::grpc::create_connection(config);

    return connection;
}


std::string get_descriptor(std::vector<std::string> proto, std::string endpoint) {
    auto connection = get_connection(endpoint);
    std::vector<std::string> proto_files;
    std::string json = auxl::grpc::describe(proto_files, &connection);

    return json;
}

/**
 */
TEST_F(AuxlGrpcTest, DISABLED_ReflectServiceTest) {
    std::vector<std::string> proto;
    auto descr = get_descriptor(proto, "localhost:4770");
    
    EXPECT_TRUE(!descr.empty());
}

/**
 */
TEST_F(AuxlGrpcTest, DISABLED_CreateTemplate) {
    std::vector<std::string> proto;
    auto descr = get_descriptor(proto, "localhost:4770");
    
    auto tpl = auxl::grpc::create_template_message("demo.EchoMessage", descr);
    
    std::cout << tpl << std::endl;
    
    EXPECT_TRUE(!tpl.empty());
}

/**
 */
TEST_F(AuxlGrpcTest, SendUnary) {
    auto connection = get_connection("localhost:4770");
    std::multimap<std::string, std::string> metadata;
    
    auto descriptors = get_descriptor({}, "localhost:4770");
    
    auto sess = Session::create(*connection, "simple.Gripmock", "SayHello", metadata, descriptors);
    sess->delegate = std::shared_ptr<SessionDelegate>(new TestSessionDelegate());

    
    // Create a message with some default values
    auto tpl = auxl::grpc::create_template_message("simple.Request", descriptors);
    
    auto p = nlohmann::json::parse(tpl);
    p["name"] = "myRequest";
    p["handle"] = "xxx";
    
    // Convert into something we can send over the wire.
    auto msg = auxl::grpc::serialized_message_from_template("simple.Request", p.dump(), descriptors);
    
    
    
    
    // Generate some test message to send.
    std::vector<std::string> tpls;
    for (int i = 0; i < 10; i++) {
        auto tpl = auxl::grpc::create_template_message("simple.Request", descriptors);
        tpls.push_back(tpl);
    }
    
    sess->start();
    // sess->send_message(msg);
    
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
            std::string tpl_to_send = tpls[selection];
            
            auto p = nlohmann::json::parse(tpl_to_send);
            p["name"] = "myRequest";
            p["handle"] = "xxx";
            
            
            std::cout << p.dump() << std::endl;
            
            auto msg = auxl::grpc::serialized_message_from_template("simple.Request", p.dump(), descriptors);
         
            sess->send_message(msg);
        }
        
    }

    sess->close();
}


}  // namespace
}  // namespace project
}  // namespace my
