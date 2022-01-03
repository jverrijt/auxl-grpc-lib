#ifndef AUXL_GRPC_CONNECTION__
#define AUXL_GRPC_CONNECTION__

#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/security/credentials.h>
#include <grpcpp/support/string_ref.h>

#include "types.h"
#include "util.h"

namespace auxl {
namespace grpc {

class Connection {
    
public:
    Connection(std::string endpoint, std::shared_ptr<::grpc::Channel> channel): endpoint(endpoint), channel(channel) {}
    
    std::string endpoint;
    std::shared_ptr<::grpc::Channel> channel;
    
    /**
     */
    static std::unique_ptr<Connection> create_connection(std::string endpoint, GRPCConnectionOptions options);
};

} // ns grpc
} // ns auxl


#endif
