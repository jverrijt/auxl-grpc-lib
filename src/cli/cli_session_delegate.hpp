//
//  cli_session_delegate.hpp
//  auxl-grpc
//
//  Created by Joost Verrijt on 06/01/2022.
//

#ifndef cli_session_delegate_hpp
#define cli_session_delegate_hpp

#include <stdio.h>
#include "session.h"
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

        std::cout << "[" << std::to_string(passed_time_in_ms) << "ms] Received response\n\n";
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
        std::cout << "Session did close with status: " <<  stat.ok() << std::endl;
    }
    
    std::chrono::system_clock::time_point message_send_time;
    
private:
    GRPCCallType call_type_;
    Descriptor* descriptor_;
    std::string output_type_name_;
};

} // ns grpc
} // ns auxl


#endif /* cli_session_delegate_hpp */
