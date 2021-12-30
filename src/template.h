
#ifndef AUXL_GRPC_TEMPLATE_H__
#define AUXL_GRPC_TEMPLATE_H__

#include <iostream>
#include <string>
#include <vector>

#include <google/protobuf/dynamic_message.h>

namespace auxl {
namespace grpc {

std::string create_template_message(std::string message_name, std::string descriptor);

/**
 */
std::string serialized_message_from_template(std::string message_name, std::string tpl, std::string descriptors);

}

}

#endif
