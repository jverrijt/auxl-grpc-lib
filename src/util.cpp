#include <stdio.h>
#include <string.h>

#include <fstream>

#include "util.h"

namespace auxl {
namespace grpc {
namespace util {

/**
 */
std::string load_file(std::string path) {
    std::ifstream in(path);
    std::string contents((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    return contents;
}

}
}
}