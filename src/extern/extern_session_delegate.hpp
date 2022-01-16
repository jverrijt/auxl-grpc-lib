//
//  extern_session_delegate.hpp
//  auxl_grpc_cli
//
//  Created by Joost Verrijt on 15/01/2022.
//

#ifndef extern_session_delegate_hpp
#define extern_session_delegate_hpp

#include <stdio.h>

#include "tool.h"

#include "session.hpp"
#include "descriptor.hpp"
#include <google/protobuf/dynamic_message.h>

namespace auxl {
namespace grpc {

class ExternSessionDelegate : public SessionDelegate
{
public:
    ExternSessionDelegate(Descriptor* descriptor, MSessionDelegate* delegate) :
        descriptor_(descriptor), delegate_(delegate) {}
    
    /**
     */
    inline void session_did_start() override
    {
        delegate_->session_did_start();
    }
    
    /**
     */
    inline void session_did_receive(std::string response, std::multimap<::grpc::string_ref, ::grpc::string_ref> meta_data) override
    {
        auto msg = descriptor_->create_message(delegate_->output_type_name);
        
        if (msg != nullptr) {
            msg->ParseFromString(response);
            std::string json_output = descriptor_->message_to_json(*msg);
        
            auto meta_data_map = create_metadata(meta_data.size());
            
            for (auto &m: meta_data) {
                metadata_push(meta_data_map, m.first.data(), m.second.data());
            }
            
            MSessionResponse *res = create_session_response((char*)json_output.c_str(), meta_data_map);
            delegate_->session_did_recieve(res);
        }
    }
    
    /**
     */
    inline void session_did_close(::grpc::Status stat, std::multimap<::grpc::string_ref, ::grpc::string_ref> metadata) override
    {
        auto meta_data_map = create_metadata(metadata.size());
        
        for (auto &m: metadata) {
            metadata_push(meta_data_map, m.first.data(), m.second.data());
        }
        

        
        // delegate_->session_did_close(
        
        // delegate_->session_did_close(-1, )
    }
    
private:
    Descriptor* descriptor_;
    MSessionDelegate* delegate_;
};



} // ns grpc
} // ns auxl



#endif /* extern_session_delegate_hpp */
