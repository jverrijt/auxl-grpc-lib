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
#include "session.hpp"
#include "parser.hpp"
#include "connection.hpp"

#include <thread>

using namespace ::grpc::testing;

namespace auxl {
namespace grpc {

/**
 */
void Session::read_response()
{
    std::string response;
    std::multimap<::grpc::string_ref, ::grpc::string_ref> incoming_meta_data;
    
    bool receive_initial_metadata = true;
    while (current_call_->ReadAndMaybeNotifyWrite(&response, receive_initial_metadata ? &incoming_meta_data : nullptr)) {
        if (delegate != nullptr) {
            delegate->session_did_receive(response, incoming_meta_data);
        }
        
        // Initial meta data received, don't notify the delegate again.
        if (!incoming_meta_data.empty()) {
            receive_initial_metadata = false;
            incoming_meta_data.clear();
        }
    }
}

/**
 */
bool Session::start(const google::protobuf::MethodDescriptor& method_descriptor)
{
    ::grpc::CliArgs args;
    args.timeout = connection_->timeout;
    
    auto m = "/" + method_descriptor.service()->full_name() + "/" + method_descriptor.name();
    current_call_ = std::unique_ptr<CliCall>(new CliCall(connection_->channel, m, metadata_, args));
    
    // CliCall failed to initialize.
    if (!current_call_->finish_status) {
        return false;
    }
    
    read_thread_ = std::thread(&Session::read_response, this);
    
    if (delegate != nullptr) {
        delegate->session_did_start();
    }
    return true;
}

/**
 */
void Session::send_message(const google::protobuf::Message& message)
{
    std::string payload;
    message.SerializeToString(&payload);
    
    // Technically the message has not yet been sent here but WriteAndWait will block.
    if (delegate != nullptr) {
        delegate->session_did_send(message);
    }
    
    current_call_->WriteAndWait(payload);
}

/**
 */
void Session::close()
{
    current_call_->WritesDoneAndWait();
    read_thread_.join();
    
    std::multimap<::grpc::string_ref, ::grpc::string_ref> metadata;
    auto stat = current_call_->Finish(&metadata);
    
    if (delegate != nullptr) {
        delegate->session_did_close(stat, metadata);
    }
}

} // ns grpc
} // ns auxl
