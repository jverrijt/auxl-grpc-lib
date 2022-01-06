//
//  cli_util.hpp
//  auxl-grpc
//
//  Created by Joost Verrijt on 06/01/2022.
//

#ifndef cli_util_hpp
#define cli_util_hpp

#include <stdio.h>

#include "descriptor.hpp"
#include "options.h"
#include "connection.h"
#include "session.h"


namespace auxl {
namespace grpc {
namespace cli {


std::shared_ptr<Connection> cli_create_connection(const std::string& endpoint, const std::string* path_to_options);

void load_and_send_message_at_path(const std::string& message_path,
                                     const std::string& message_type_name,
                                     Descriptor& descriptors,
                                   Session& session);

} // ns cli
} // ns grpc
} // ns auxl


#endif /* cli_util_hpp */
