//
//  session.hpp
//  auxl_grpc_cli
//
//  Created by Joost Verrijt on 28/12/2021.
//

#ifndef session_h
#define session_h

#include <stdio.h>
#include <memory>
#include <thread>

#include <google/protobuf/dynamic_message.h>
#include "test/cpp/util/cli_call.h"
#include "connection.h"
#include "parser.h"

namespace auxl {
namespace grpc {



class SessionDelegate {
public:
    virtual void did_receive(std::string response, std::multimap<::grpc::string_ref, ::grpc::string_ref> meta_data) {};
};


class Session {
    
public:
    
    std::shared_ptr<SessionDelegate> delegate;
    
    /**
     Create a session
     */
    static std::shared_ptr<Session> create(Connection& connection,
                                            std::string service_name,
                                            std::string method_name,
                                            std::multimap<std::string, std::string> metadata,
                                            std::string descriptors,
                                            double timeout = -1);
    
    void start();
    
    /**
     */
    void send_message(std::string& message);
    
    /**
     */
    void send_message(std::shared_ptr<google::protobuf::Message> message);
    
    void close();
    
    bool is_streaming();
    
    /**
     Returns the full method name in the form of service/method
     */
    std::string method_name();
    
private:
    Session(Connection& connection,
            std::string method_name,
            bool is_streaming,
            std::multimap<std::string, std::string> metadata,
            ::grpc::CliArgs args);
    
    void read_response(bool* should_read);
    
    bool _streaming;
    std::string _method_name;
    
    std::thread read_thread;
    bool should_read;
    
    std::unique_ptr<::grpc::testing::CliCall> call;
};

} // ns grpc
} // ns auxl




#endif /* session_hpp */
