
#include "connection.h"
#include "util.h"

#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/security/credentials.h>
#include <grpcpp/support/string_ref.h>

#include <grpc/slice.h>
#include <grpc/support/log.h>
#include <grpcpp/impl/codegen/slice.h>

using namespace ::grpc;

namespace auxl {
namespace grpc {

/**
*/ 
std::shared_ptr<ChannelCredentials> get_channel_credentials(GRPCConfig config)
{
    if (!config.options.use_ssl) {
        return InsecureChannelCredentials();
    }
    else if (config.options.use_ssl) {
        SslCredentialsOptions ssl_creds_options;
        // TODO
        ssl_creds_options.pem_root_certs = util::load_file(config.options.ssl_root_certs_path);

        if (config.options.ssl_client_cert != NULL) {
            // FIXME, what if this fails?
            ssl_creds_options.pem_cert_chain = util::load_file(config.options.ssl_client_cert);
        }
        
        if (config.options.ssl_client_key != NULL) {
            ssl_creds_options.pem_private_key = util::load_file(config.options.ssl_client_key);
        }

        return SslCredentials(ssl_creds_options);
    }

    return std::shared_ptr<ChannelCredentials>();
}

/**
 */
std::shared_ptr<Connection> create_connection(GRPCConfig config)
{
    ChannelArguments args;

    args.SetInt(GRPC_ARG_MAX_METADATA_SIZE, 10 * 1024 * 1024);
    auto channel = CreateCustomChannel(config.endpoint, get_channel_credentials(config), args);

    std::shared_ptr<Connection> connection(new Connection);
    connection->channel = channel;

    return connection;
}
}

}
