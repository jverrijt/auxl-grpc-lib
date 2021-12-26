
#include "convertor.h"
#include <nlohmann/json.hpp>

using namespace nlohmann;

namespace auxl {
namespace grpc {

void create_template_message(std::string message_name, std::string descriptor) 
{
    // Find the message in the given descriptor
    json o = json::parse(descriptor);
    
    for (json::iterator it = o.begin(); it != o.end(); ++it) {
        std::cout << it.key() << " : " << it.value() << "\n";
    }
}


}
}
