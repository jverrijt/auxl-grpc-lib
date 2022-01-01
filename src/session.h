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
#include "test/cpp/util/cli_call.h"
#include "parser.h"
#include "descriptor.hpp"
#include "types.h"


namespace auxl {
namespace grpc {

class SessionDelegate {
public:
    virtual void did_receive(std::string response, std::multimap<::grpc::string_ref, ::grpc::string_ref> meta_data) {};
};


class Session {
    
public:
    Session(std::shared_ptr<auxl::grpc::Connection> connection): connection_(connection) {};
    
    std::weak_ptr<SessionDelegate> delegate;
    
    void start();
    
    /**
     */
    void send_message(google::protobuf::Message& message, const google::protobuf::MethodDescriptor& method_descriptor,
                      std::multimap<std::string, std::string> metadata = {}, double timeout = -1);
    
    
    void close();
    
private:

    void read_response();
    
    std::thread read_thread_;
    
    std::shared_ptr<auxl::grpc::Connection> connection_;
    std::unique_ptr<::grpc::testing::CliCall> current_call_;
};

} // ns grpc
} // ns auxl




#endif /* session_hpp */
