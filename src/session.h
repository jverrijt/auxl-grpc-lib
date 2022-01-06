//
//  session.hpp
//  auxl_grpc_cli
//
//  Created by Joost Verrijt on 28/12/2021.
//

#ifndef session_h
#define session_h

#include <stdio.h>
#include <memory>
#include <thread>

#include <google/protobuf/dynamic_message.h>
#include "cli_call.h"
#include "parser.h"
#include "descriptor.hpp"
#include "types.h"


namespace auxl {
namespace grpc {

class Session;

class SessionDelegate
{
public:
    virtual void session_did_start() = 0;
    
    virtual void session_did_send(const google::protobuf::Message& message) {};
    
    virtual void session_did_receive(std::string response,
                             std::multimap<::grpc::string_ref, ::grpc::string_ref> meta_data) = 0;
    
    virtual void session_did_close(::grpc::Status stat,
                                   std::multimap<::grpc::string_ref, ::grpc::string_ref> metadata) = 0;
};


class Session
{
public:
    Session(const Connection* connection): connection_(connection) {
        delegate = nullptr;
    };
    
    void start(const google::protobuf::MethodDescriptor& method_descriptor,
              std::multimap<std::string, std::string> metadata = {} , double timeout = -1);
    
    /**
     */
    void send_message(const google::protobuf::Message& message);
    
    void close();
    
    SessionDelegate* delegate;
    
private:
    void read_response();
    
    std::thread read_thread_;
    
    const Connection* connection_;
    
    std::unique_ptr<::grpc::testing::CliCall> current_call_;
};

} // ns grpc
} // ns auxl


#endif /* session_hpp */
