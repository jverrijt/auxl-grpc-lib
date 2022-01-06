
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


/**
 Splits metadata in string form (key:val,key:val,...) into key, value pairs
 */
void parse_meta_data(std::map<std::string, std::string>& map, const std::string& text);


} // ns util
} // ns grpc
} // ns auxl
