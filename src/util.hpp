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

#ifndef AUXL_GRPC_UTIL_HPP
#define AUXL_GRPC_UTIL_HPP

#include <tuple>
#include <string>
#include "options.h"
#include "types.h"
#include <google/protobuf/dynamic_message.h>

namespace auxl {
namespace grpc {
namespace util {

/**
 * Load file contents into a string
 */
bool load_file(const std::string& path, std::string* in_string);

/**
 Returns constructed connection options from given json.
 */
GRPCConnectionOptions* options_from_json(const std::string& json);


/**
 Split a given string into its package.service and method components
 */
std::tuple<bool, std::string, std::string> split_service_method(const std::string& input);


/**
 Returns GRPC call type (i.e. unary, clientstreaming etc) and textual name.
 */
GRPCCallInfo call_info(const google::protobuf::MethodDescriptor& descriptor);


} // ns util
} // ns grpc
} // ns auxl


#endif
