
#ifndef AUXL_GRPC_PARSER__
#define AUXL_GRPC_PARSER__

#include <vector>
#include <exception>

#include <google/protobuf/compiler/importer.h>

#include "connection.h"
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
std::shared_ptr<google::protobuf::DescriptorDatabase> parse_descriptors(std::string descriptors);


} // ns grpc
} // ns auxl

#endif
