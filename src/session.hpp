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

#ifndef session_h
#define session_h

#include <stdio.h>
#include <memory>
#include <thread>

#include <google/protobuf/dynamic_message.h>
#include "cli_call.h"
#include "parser.hpp"
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
    
    /**
     */
    bool start(const google::protobuf::MethodDescriptor& method_descriptor,
               const std::multimap<std::string, std::string>& metadata = {});
    
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
