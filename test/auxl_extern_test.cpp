//
//  auxl_extern_test.cpp
//  auxl_grpc_cli
//
//  Created by Joost Verrijt on 15/01/2022.
//
#include "gtest/gtest.h"
#include "tool.h"

namespace auxl {
namespace grpc {
namespace {

class AuxlExternTest : public ::testing::Test {
};


// delegate methods

void session_did_start() {
    printf("The session did indeed start\n");
}

void session_did_send(MMessageHandle message) {
    printf("Message did send\n");
}

void session_did_recieve(MSessionResponse* response) {
    printf("Got response %s", response->response);
}

void session_did_close(MMetadata* closing_metadata) {
    printf("The session did indeed close\n");
}


TEST_F(AuxlExternTest, TestCInterface)
{
    GRPCConnectionOptions def = init_connection_options();
    MConnectionHandle connection = create_connection((char*) "localhost:5000", def);
    
    MDescriptorHandle descriptor = create_descriptor(NULL, 0, connection);
    
    MSessionDelegate* delegate = create_session_delegate((char*)"HelloResponse", &session_did_start, &session_did_send, &session_did_recieve, &session_did_close);
    
    MSessionHandle session = create_session(connection, descriptor, delegate);
    
    char* json_result = descriptor_to_json(descriptor);
    
    
    if (session_start(session, descriptor, (char*) "greet.Greeter.SayHello") != -1) {
        
        
        
    }
    
    
    
    
    printf("%s", json_result);
    
    free(json_result);
    
    free_connection(connection);
    free_descriptor(descriptor);
    free_session(session);
    free_session_delegate(delegate);
    
    printf("We're done\n");
}

}
} // ns grpc
} // ns auxl
