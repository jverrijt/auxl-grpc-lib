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

#ifndef descriptor_session_delegate_hpp
#define descriptor_session_delegate_hpp

#include "auxl_grpc.h"
#include <stdio.h>

#include "session.hpp"
#include "descriptor.hpp"
#include <google/protobuf/dynamic_message.h>


AUXLGRPC_NS_BEGIN

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



AUXLGRPC_NS_END


#endif /* descriptor_session_delegate_hpp */
