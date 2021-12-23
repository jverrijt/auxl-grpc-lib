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
std::string load_file(std::string path) {
    std::ifstream in(path);

    if (!in) {
        std::cout <<  "Got error while opening file";
    }

    std::string contents((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());


    return contents;
}

}
}
}