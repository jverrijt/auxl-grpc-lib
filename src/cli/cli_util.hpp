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

#ifndef cli_util_hpp
#define cli_util_hpp

#include <stdio.h>

#include "descriptor.hpp"
#include "options.h"
#include "connection.hpp"
#include "session.hpp"


namespace auxl {
namespace grpc {
namespace cli {


std::shared_ptr<Connection> cli_create_connection(const std::string& endpoint, const std::string* path_to_options);

void load_and_send_message_at_path(const std::string& message_path,
                                   const std::string& message_type_name,
                                   Descriptor& descriptors,
                                   Session& session);

/**
 Parse meta data into map.
 */
void parse_metadata(const std::vector<std::string>& key_val_pairs, std::multimap<std::string, std::string>& map);

} // ns cli
} // ns grpc
} // ns auxl


#endif /* cli_util_hpp */
