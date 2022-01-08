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

#ifndef AUXL_GRPC_PARSER__
#define AUXL_GRPC_PARSER__

#include <vector>
#include <exception>

#include <google/protobuf/compiler/importer.h>

#include "connection.hpp"
#include "types.h"
#include "error_collector.h"

namespace auxl {
namespace grpc {

/**
 Retrieve the descriptors from a  list of proto files and place them in the given Descriptor database
 */
void descriptors_from_proto_files(std::vector<std::string> proto_files,
                                  google::protobuf::SimpleDescriptorDatabase* db, AuxlGRPCErrorCollector *error_collector);

/**
 Retrieve the descriptors from a reflection source and place them in the given Descriptor database
 */
void descriptors_from_reflect(const Connection& connection,
                              google::protobuf::SimpleDescriptorDatabase* db, AuxlGRPCErrorCollector *error_collector);

/**
 Parse the given descriptors in Json format into a descriptor databse
 */
std::unique_ptr<google::protobuf::SimpleDescriptorDatabase> parse_descriptors(const std::string& descriptors);


} // ns grpc
} // ns auxl

#endif
