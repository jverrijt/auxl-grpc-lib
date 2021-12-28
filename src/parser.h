
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
 Returns the descriptors contained in the given proto files and/or retrieval fro, server reflection.
 */
std::string describe(std::vector<std::string> proto_files, std::shared_ptr<Connection>* connection);

/**
 Parse the given descriptors in Json format into a descriptor databse
 */
std::shared_ptr<google::protobuf::DescriptorDatabase> parse_descriptors(std::string descriptors);

} // ns grpc
} // ns auxl

#endif
