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

#include "connection.hpp"
#include "util.hpp"

#include <grpc/slice.h>
#include <grpc/support/log.h>
#include <grpcpp/impl/codegen/slice.h>

using namespace ::grpc;

namespace auxl {
namespace grpc {

/**
*/ 
std::shared_ptr<ChannelCredentials> Connection::get_channel_credentials(GRPCConnectionOptions options)
{
    if (!options.use_ssl) {
        return InsecureChannelCredentials();
    }
    else if (options.use_ssl) {
        SslCredentialsOptions ssl_creds_options;
        
        if (options.ssl_root_certs_path != NULL) {
            std::string ssl_roots_certs;
            if (util::load_file(options.ssl_root_certs_path, &ssl_roots_certs)) {
                ssl_creds_options.pem_root_certs = ssl_roots_certs;
            }
        }

        if (options.ssl_client_cert != NULL) {
            std::string ssl_client_cert;
            if (util::load_file(options.ssl_client_cert, &ssl_client_cert)) {
                ssl_creds_options.pem_cert_chain = ssl_client_cert;
            }
        }
        
        if (options.ssl_client_key != NULL) {
            std::string ssl_client_key;
            if (util::load_file(options.ssl_client_key, &ssl_client_key)) {
                ssl_creds_options.pem_private_key = ssl_client_key;
            }
        }

        return SslCredentials(ssl_creds_options);
    }

    return std::shared_ptr<ChannelCredentials>();
}

/**
 */
std::unique_ptr<Connection> Connection::create_connection(const std::string& endpoint, GRPCConnectionOptions options)
{
    if (options.timeout == 0) {
        std::cerr << "Warning, timeout is set to 0. This may result undefined behavior." << std::endl;
    }
    
    ChannelArguments args;

    args.SetInt(GRPC_ARG_MAX_METADATA_SIZE, 10 * 1024 * 1024);
    auto channel = CreateCustomChannel(endpoint, get_channel_credentials(options), args);

    std::unique_ptr<Connection> connection(new Connection(endpoint, channel));
    
    connection->timeout = options.timeout;
    
    return connection;
}

} // ns grpc
} // ns auxl
