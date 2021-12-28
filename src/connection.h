#ifndef AUXL_GRPC_CONNECTION__
#define AUXL_GRPC_CONNECTION__

#include <grpcpp/channel.h>
#include "types.h"

namespace auxl {
namespace grpc {

struct Connection {
    std::shared_ptr<::grpc::Channel> channel;
};

std::shared_ptr<Connection> create_connection(GRPCConfig config);

}
}


#endif
