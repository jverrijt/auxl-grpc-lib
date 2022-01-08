/*
 *
 * Copyright 2022 Joost Verrijt.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef cli_session_delegate_hpp
#define cli_session_delegate_hpp

#include <stdio.h>
#include "session.hpp"
#include "types.h"
#include <string>

namespace auxl {
namespace grpc {

class CliSessionDelegate : public SessionDelegate
{
public:
    CliSessionDelegate(Descriptor *descriptor, std::string output_type_name, GRPCCallType call_type) :
        descriptor_(descriptor), output_type_name_(output_type_name), call_type_(call_type) { }
    
    /**
     */
    inline void session_did_start() override { }
    
    /**
     */
    inline void session_did_send(const google::protobuf::Message& message) override
    {
        message_send_time = std::chrono::system_clock::now();
    };
    
    /**
     */
    inline void session_did_receive(std::string response, std::multimap<::grpc::string_ref, ::grpc::string_ref> meta_data) override
    {
        using namespace std::chrono;
        
        auto passed_time_in_ms = duration_cast<milliseconds>(std::chrono::system_clock::now() - message_send_time).count();
        message_send_time = system_clock::now();

        std::cout << "[" << std::to_string(passed_time_in_ms) << "ms] Received response " << std::to_string(++response_count) << "\n\n";
        auto msg = descriptor_->create_message(output_type_name_);
        
        if (!meta_data.empty()) {
            std::cout << "Metadata:" << std::endl;
            
            for (auto &m: meta_data) {
                std::cout << m.first << ": " << m.second << std::endl;
            }
            
            std::cout << std::endl;
        }
        
        if (msg != nullptr) {
            msg->ParseFromString(response);
            std::string json_output = descriptor_->message_to_json(*msg);
            std::cout << json_output << std::endl;
        }
        
        // is there more to come?
        if (call_type_ == ClientStreaming || call_type_ == BidirectionalStreaming) {
            std::cout << "Enter # of message to send or q to quit." << std::endl;
        }
    }
    
    /**
     */
    inline void session_did_close(::grpc::Status stat, std::multimap<::grpc::string_ref, ::grpc::string_ref> metadata) override
    {
        if (stat.ok()) {
            std::cout << "Session closed normally." << std::endl;
        } else {
            std::cerr << "Session closed with error: " << stat.error_message() << std::endl;
        }
    }
    
    int response_count = 0;
    std::chrono::system_clock::time_point message_send_time;
    
private:
    GRPCCallType call_type_;
    Descriptor* descriptor_;
    std::string output_type_name_;
};

} // ns grpc
} // ns auxl


#endif /* cli_session_delegate_hpp */
