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

#include "insecure_certificate_provider.h"

using namespace ::grpc;

AUXLGRPC_NS_BEGIN

/**
*/ 
std::shared_ptr<ChannelCredentials> Connection::get_channel_credentials(GRPCConnectionOptions options)
{
    if (options.enable_tls) {
        ::grpc::experimental::TlsChannelCredentialsOptions opts;

        std::string in_str;
        util::load_file(options.pem_root_certs_path, &in_str);
        
        auto certificate_provider = std::make_shared<::grpc::experimental::StaticDataCertificateProvider>(in_str);
        opts.set_certificate_provider(certificate_provider);

        if (options.validate_tls == false) {
            opts.set_check_call_host(false);
            opts.set_verify_server_certs(false);
            
            auto verifier = experimental::ExternalCertificateVerifier::Create<InsecureCertificateVerifier>();
            opts.set_certificate_verifier(verifier);
        }

        return TlsCredentials(opts);
    } else {
        return InsecureChannelCredentials();
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

AUXLGRPC_NS_END
