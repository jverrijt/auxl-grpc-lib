//
//  descriptor_session_delegate.hpp
//  auxl-grpc
//
//  Created by Joost Verrijt on 03/01/2022.
//

#ifndef descriptor_session_delegate_hpp
#define descriptor_session_delegate_hpp

#include <stdio.h>

#include "session.h"
#include "descriptor.hpp"
#include <google/protobuf/dynamic_message.h>


namespace auxl {
namespace grpc {

struct SessionResponse {
    
}; 

class DescriptorSessionDelegate : public SessionDelegate
{
public:
    DescriptorSessionDelegate(Descriptor *descriptor, std::string output_type_name) :
        descriptor_(descriptor), output_type_name_(output_type_name) {}
    
    /**
     */
    inline void session_did_start() override
    {
        std::cout << "Session did start at:" << time(NULL) << std::endl;
    }
    
    /**
     */
    inline void session_did_receive(std::string response, std::multimap<::grpc::string_ref, ::grpc::string_ref> meta_data) override
    {
        std::cout << "Delegate received : " << response << std::endl;
        auto msg = descriptor_->create_message(output_type_name_);
        
        if (msg != nullptr) {
            msg->ParseFromString(response);
            std::string json_output = descriptor_->message_to_json(*msg);
            std::cout << json_output << std::endl;
        }
    }
    
    /**
     */
    inline void session_did_close(::grpc::Status stat, std::multimap<::grpc::string_ref, ::grpc::string_ref> metadata) override
    {
        std::cout << "Session did close with status: " <<  stat.ok() << std::endl;
    }
    
private:
    Descriptor* descriptor_;
    std::string output_type_name_;
};



} // ns grpc
} // ns auxl


#endif /* descriptor_session_delegate_hpp */
