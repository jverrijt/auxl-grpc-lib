
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
 * Converts proto files to a FileDescriptor JSON format.
 */
std::string proto_files_to_fd_json(std::vector<std::string> proto_files);

/**
 * Temp
 */
std::string grpc_reflect(std::shared_ptr<Connection>& connection);

} // auxl
} // grpc

#endif