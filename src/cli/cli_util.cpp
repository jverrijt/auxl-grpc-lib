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
#include "cli_util.hpp"

namespace auxl {
namespace grpc {
namespace cli {

/**
 Create connection with loading options from a given path.
 */
std::shared_ptr<Connection> cli_create_connection(const std::string& endpoint, const std::string* path_to_options)
{
    std::shared_ptr<Connection> connection;
    GRPCConnectionOptions *opts = connection_options_default();
    
    if (path_to_options != NULL) {
        std::string connection_opts_json;
        
        if (util::load_file(*path_to_options, &connection_opts_json) && !connection_opts_json.empty()) {
            connection_options_free(opts);
            opts = util::options_from_json(connection_opts_json);
        }
    }
    
    auto conn = Connection::create_connection(endpoint, *opts);
    
    connection = std::move(conn);
    connection_options_free(opts);
    
    return connection;
}

/**
 */
void load_and_send_message_at_path(const std::string& message_path,
                                   const std::string& message_type_name,
                                   Descriptor& descriptors,
                                   Session& session)
{
    std::string json_message;
    if (util::load_file(message_path, &json_message)) {
        auto message = descriptors.message_from_json(message_type_name, json_message);
        session.send_message(*message);
    } else {
        std::cerr << "Could not load message at " << message_path << std::endl;
    }
}

/**
 */
void parse_metadata(const std::vector<std::string>& key_val_pairs, std::multimap<std::string, std::string>& map)
{
    for (std::string metadata : key_val_pairs) {
        
        auto found = metadata.find_last_of(":");
        
        if (found != std::string::npos) {
            std::string key = metadata.substr(0, found);
            std::string val = metadata.substr(found + 1, std::string::npos);
            
            map.insert(std::pair<std::string, std::string>(key, val));
        }
    }
}

} // ns cli
} // ns grpc
} // ns auxl
