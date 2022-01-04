
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

} // ns util
} // ns grpc
} // ns auxl
