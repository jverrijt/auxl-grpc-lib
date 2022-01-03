
#include <string>

namespace auxl {
namespace grpc {
namespace util {

/**
 * Load file contents into a string
 */
bool load_file(const std::string& path, std::string* in_string);


} // ns util
} // ns grpc
} // ns auxl
