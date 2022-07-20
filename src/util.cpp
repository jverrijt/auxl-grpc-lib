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

#include <stdio.h>
#include <iostream>
#include <string>

#include <fstream>

#include <nlohmann/json.hpp>

#include "util.hpp"

namespace auxl {
namespace grpc {
namespace util {

/**
 */
bool load_file(const std::string& path, std::string* in_string) {
    std::ifstream in(path);
    
    if (!in) {
        std::cerr <<  "Unable to open file:" << path << std::endl;
        return false;
    }
    
    std::string contents((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    
    in_string->assign(contents);
    
    return true;
}

/**
 */
GRPCConnectionOptions* options_from_json(const std::string& json)
{
    try {
        auto o = nlohmann::json::parse(json);
        
        bool enable_tls = o.contains("enable_tls") ? o["enable_tls"].get<bool>() : false;
        bool validate_tls = o.contains("validate_tls") ? o["validate_tls"].get<bool>() : true;
        
        double timeout = o.contains("timeout") ? o["timeout"].get<double>() : -1;
        std::string pem_root_certs_path = o.contains("pem_root_certs_path") ? o["pem_root_certs_path"].get<std::string>() : "";
        
        GRPCConnectionOptions *opts = connection_options_create(timeout,
                                                                enable_tls,
                                                                validate_tls,
                                                                (char*) pem_root_certs_path.c_str());
        return opts;
    } catch(...) {
        std::cerr << "Cold not parse connection options, returning defaults.";
        
        return connection_options_default();
    }
}

/**
 */
std::tuple<bool, std::string, std::string> split_service_method(const std::string& input)
{
    auto lp = input.find_last_of(".");
    
    std::string service_part;
    std::string method_part;
    
    if(lp == std::string::npos) {
        // invalid
        return std::make_tuple(false, "", "");
    }
    
    service_part = input.substr(0, lp);
    method_part = input.substr(lp + 1, std::string::npos);
    
    return std::make_tuple(true, service_part, method_part);
}

/**
 */
GRPCCallInfo call_info(const google::protobuf::MethodDescriptor& descriptor)
{
    GRPCCallInfo info = { };
    
    if (descriptor.client_streaming() && descriptor.server_streaming()) {
        info.name =  (char*) "bidirectional streaming";
        info.type = BidirectionalStreaming;
    } else if (descriptor.client_streaming() && !descriptor.server_streaming()) {
        info.name =  (char*) "client streaming";
        info.type = ClientStreaming;
    } else if (!descriptor.client_streaming() && descriptor.server_streaming()) {
        info.name =  (char*) "server streaming";
        info.type = ServerStreaming;
    } else {
        info.name =  (char*) "unary";
        info.type = Unary;
    }
    return info;
}

} // ns util
} // ns grpc
} // ns auxl
