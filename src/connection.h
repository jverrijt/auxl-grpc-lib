#ifndef AUXL_GRPC_CONNECTION__
#define AUXL_GRPC_CONNECTION__

#include <grpcpp/channel.h>
#include "types.h"

namespace auxl {
namespace grpc {

struct Connection {
    std::string endpoint;
    std::shared_ptr<::grpc::Channel> channel;
};

std::unique_ptr<Connection> create_connection(std::string endpoint, GRPCConnectionOptions options);

}
}


#endif
