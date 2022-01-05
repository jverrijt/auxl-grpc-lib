
#include <tuple>
#include <string>
#include "options.h"

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

} // ns util
} // ns grpc
} // ns auxl
