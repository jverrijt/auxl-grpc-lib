
#ifndef AUXL_GRPC_PARSER__
#define AUXL_GRPC_PARSER__

#include <vector>
#include <exception>

#include <google/protobuf/compiler/importer.h>

#include "connection.h"
#include "types.h"

namespace auxl {
namespace grpc {

typedef enum {
    CouldNotFindFile = 1
} AuxlGRPCParserError;


class ProtoFileParserError : public google::protobuf::compiler::MultiFileErrorCollector {
 public:
    ProtoFileParserError() {}
    ~ProtoFileParserError() {}

    std::vector<ParserErrorEntry> entries;

  void AddError(const std::string& filename, int line, int column,
                const std::string& message) override;

  void AddWarning(const std::string& filename, int line, int column,
                  const std::string& message) override;
};


/// Auxl grpc exception Exception
struct AuxlGRPCParserException : std::exception {
    const char* message;
    const AuxlGRPCParserError error_code;

    public: 
        AuxlGRPCParserException(char* message, AuxlGRPCParserError error_code): 
        message(message), 
        error_code(error_code) {}
};


/**
 Retrieve the descriptors from a  list of proto files and place them in the given Descriptor database
 */
void descriptors_from_proto_files(std::vector<std::string> proto_files, google::protobuf::SimpleDescriptorDatabase* db);

/**
 Retrieve the descriptors from a reflection source and place them in the given Descriptor database
 */
void descriptors_from_reflect(Connection& connection, google::protobuf::SimpleDescriptorDatabase* db);

/**
 Parse the given descriptors in Json format into a descriptor databse
 */
std::shared_ptr<google::protobuf::DescriptorDatabase> parse_descriptors(std::string descriptors);


} // ns grpc
} // ns auxl

#endif
