//
//  session.cpp
//  auxl_grpc_cli
//
//  Created by Joost Verrijt on 28/12/2021.
//

#include "session.h"
#include "parser.h"
#include "connection.h"

#include <thread>

using namespace ::grpc::testing;

namespace auxl {
namespace grpc {

/**
 */
void Session::read_response()
{
    std::string response;
    std::multimap<::grpc::string_ref, ::grpc::string_ref> incoming_meta_data;
    
    for (bool receive_initial_metadata = true; current_call_->ReadAndMaybeNotifyWrite(&response,
                                                                             receive_initial_metadata ? &incoming_meta_data : nullptr);
         receive_initial_metadata = false) {
        fprintf(stderr, "got response. %s\n", response.c_str());
       
    }
        
    std::cout << "Ending thread..." << std::endl;
}


void Session::start(const google::protobuf::MethodDescriptor& method_descriptor, std::multimap<std::string, std::string> metadata, double timeout)
{
    auto m = "/" + method_descriptor.service()->full_name() + "/" + method_descriptor.name();
    
    current_call_ = std::unique_ptr<CliCall>(new CliCall(connection_->channel, m, metadata));
    
    // Start the read thread.
    read_thread_ = std::thread(&Session::read_response, this);
}

/**
 */
void Session::send_message(google::protobuf::Message& message)
{
    std::string payload;
    message.SerializeToString(&payload);
    
    current_call_->WriteAndWait(payload);
}

/**
 */
void Session::close()
{
    current_call_->WritesDoneAndWait();
    read_thread_.join();
    
    std::multimap<::grpc::string_ref, ::grpc::string_ref> metadata;
    auto stat = current_call_->Finish(&metadata);
    
    std::cout << "Received error: " << stat.error_message() << std::endl;
}


} // ns grpc
} // ns auxl
