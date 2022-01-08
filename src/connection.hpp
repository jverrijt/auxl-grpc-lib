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

#ifndef AUXL_GRPC_CONNECTION__
#define AUXL_GRPC_CONNECTION__

#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/security/credentials.h>
#include <grpcpp/support/string_ref.h>

#include "types.h"
#include "util.hpp"

namespace auxl {
namespace grpc {

class Connection {
    
public:
    Connection(const std::string& endpoint, std::shared_ptr<::grpc::Channel> channel): endpoint(endpoint), channel(channel) {}
    
    const std::string endpoint;
    std::shared_ptr<::grpc::Channel> channel;
    
    /**
     */
    static std::shared_ptr<::grpc::ChannelCredentials> get_channel_credentials(GRPCConnectionOptions options);
    
    /**
     */
    static std::unique_ptr<Connection> create_connection(const std::string& endpoint, GRPCConnectionOptions options);
    
    double timeout = -1;
};

} // ns grpc
} // ns auxl


#endif
