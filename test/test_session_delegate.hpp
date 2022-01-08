//
//  TestSessionDelegate.hpp
//  auxl_grpc_test
//
//  Created by Joost Verrijt on 08/01/2022.
//

#ifndef test_session_delegate_hpp
#define test_session_delegate_hpp

#include <stdio.h>
#include "gtest/gtest.h"

#include "session.hpp"
#include "descriptor.hpp"

class TestSessionDelegate: public auxl::grpc::SessionDelegate {

public:
    TestSessionDelegate(auxl::grpc::Descriptor *descriptor, std::string output_type_name) :
        descriptor_(descriptor), type_name_(output_type_name) {}
    
    inline void session_did_start() override {
        std::cout << "Session did start at:" << time(NULL) << std::endl;
    }
    
    void session_did_receive(std::string response, std::multimap<::grpc::string_ref, ::grpc::string_ref> meta_data) override {
        std::cout << "Delegate received : " << response << std::endl;
        
        auto msg = descriptor_->create_message(type_name_);
        
        // Assert if we recieved a message and if this message is of the expected type.
        
        ASSERT_TRUE(msg != nullptr);
        
        msg->ParseFromString(response);
        
        ASSERT_TRUE(msg->GetTypeName() == type_name_);
    
        std::string json_output = descriptor_->message_to_json(*msg);
        std::cout << json_output << std::endl;
        
        received_messages++;
    }

    void session_did_close(::grpc::Status stat, std::multimap<::grpc::string_ref, ::grpc::string_ref> metadata) override
    {
        std::cout << "Session did close with status: " <<  stat.ok() << std::endl;
    }

    int received_messages = 0;

private:
    auxl::grpc::Descriptor* descriptor_;
    std::string type_name_;
    
};

#endif /* test_session_delegate_hpp */
