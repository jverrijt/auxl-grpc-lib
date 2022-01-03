#include <stdio.h>
#include <iostream>
#include <string>

#include <fstream>

#include "util.h"

namespace auxl {
namespace grpc {
namespace util {

/**
 */
bool load_file(const std::string& path, std::string* in_string) {
    std::ifstream in(path);

    if (!in) {
        std::cerr <<  "Unable to open file:" << path << std::endl;
        return false;
    }

    std::string contents((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    
    in_string = &contents;

    return true;
}

}
}
}
