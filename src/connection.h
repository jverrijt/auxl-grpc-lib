#ifndef AUXL_GRPC_CONNECTION__
#define AUXL_GRPC_CONNECTION__

#include <grpcpp/channel.h>
#include "types.h"

namespace auxl {
namespace grpc {

struct Connection {
    ::grpc::Channel* channel;
};

std::unique_ptr<Connection> create_connection(GRPCConfig config);

}
}


#endif