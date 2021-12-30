//
//  session.cpp
//  auxl_grpc_cli
//
//  Created by Joost Verrijt on 28/12/2021.
//

#include "session.h"
#include "parser.h"
#include <thread>

using namespace ::grpc::testing;

namespace auxl {
namespace grpc {

/**
 */
Session::Session(Connection& connection,
                 std::string method_name,
                 bool is_streaming,
                 std::multimap<std::string, std::string>
                 metadata, ::grpc::CliArgs args) {

    delegate = nullptr;
    
    _method_name = method_name;
    _streaming = is_streaming;

    call = std::unique_ptr<CliCall>(new ::grpc::testing::CliCall(connection.channel, _method_name, metadata));
}

/**
 */
bool Session::is_streaming()
{
    return _streaming;
}

/**
 Returns the full method name in the form of service/method
 */
std::string Session::method_name()
{
    return _method_name;
}

/**
 */
std::shared_ptr<Session> Session::create(Connection& connection,
                                        std::string service_name,
                                        std::string method_name,
                                        std::multimap<std::string, std::string> metadata,
                                        std::string descriptors,
                                        double timeout)
{
    // Make sure the descriptors work and contain the given service and method.
    auto db = parse_descriptors(descriptors);
    google::protobuf::DescriptorPool pool(db.get());
        
    auto service_descr = pool.FindServiceByName(service_name);
    
    if (service_descr == nullptr) {
        throw -1;
    }
    
    auto method_descr = service_descr->FindMethodByName(method_name);
    
    if (method_descr == nullptr) {
        throw -1;
    }
    
    ::grpc::CliArgs args;
    args.timeout = timeout;
    
    // Creat a service/method name that works for CliCall
    auto m = "/" + method_descr->service()->full_name() + "/" + method_descr->name();
    
    std::cout << "Method to call:" << m << std::endl;
    bool is_streaming = method_descr->server_streaming() || method_descr->client_streaming();
    
    std::shared_ptr<Session> session(new Session(connection, m, is_streaming, metadata, args));
    
    return session;
}

/**
 */
void Session::read_response(bool* should_read)
{
    while(*should_read) {
        std::string response;
        std::multimap<::grpc::string_ref, ::grpc::string_ref> incoming_meta_data;
        
        // call->ReadAndMaybeNotifyWrite(&response, &incoming_meta_data);
        
        call->Read(&response, &incoming_meta_data);
        
        if (delegate != nullptr) {
            delegate->did_receive(response, incoming_meta_data);
        }
        
        sleep(1);
    }
    
    std::cout << "Ending thread..." << std::endl;
}

/**
 */
void Session::start()
{
    should_read = true;
    read_thread = std::thread(&Session::read_response, this, &should_read);
    
    std::cout << "Starting ..." << std::endl;
}

/**
 */
void Session::send_message(std::string& message) {
    call->Write(message);
}

/**
 */
void Session::close()
{
    should_read = false;
    
    call->WritesDone();
    read_thread.join();
    
    std::multimap<::grpc::string_ref, ::grpc::string_ref> metadata;
    auto stat = call->Finish(&metadata);
    
    std::cout << "Received error: " << stat.error_message() << std::endl;
}


} // ns grpc
} // ns auxl
