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
    printf("Metadata:\n");
    for (int i = 0; i < response->metadata->count; i++) {
        printf("Key: %s, val: %s\n", response->metadata->keys[i], response->metadata->vals[i]);
    }
    printf("Got response %s", response->response);
    free_session_response(response);
}

void session_did_close(MSessionResponse* response, MMetadata* closing_metadata) {
    printf("The session did indeed close\n");
    
    free_session_response(response);
}


TEST_F(AuxlExternTest, TestCInterface)
{
    GRPCConnectionOptions def = init_connection_options();
    MConnectionHandle connection = create_connection((char*) "localhost:5000", def);
    
    MDescriptorHandle descriptor = create_descriptor(NULL, 0, connection);
    
    MSessionDelegate delegate = {
        .output_type_name = (char*) "HelloResponse",
        .session_did_start = &session_did_start,
        .session_did_send = &session_did_send,
        .session_did_recieve = &session_did_recieve,
        .session_did_close = &session_did_close
    };
    
    MSessionHandle session = create_session(connection, descriptor, &delegate);
    
    char* json_result = descriptor_to_json(descriptor);
    
    if (session_start(session, descriptor, (char*) "greet.Greeter.SayHello") != -1) {
        char* hello_request_msg = descriptor_create_json_message(descriptor, "greet.HelloRequest");
        //session_send_message(hello_request_msg, "greet.HelloRequest");
        
        
    }
    
    
    
    
    
    
    printf("%s", json_result);
    
    free(json_result);
    
    free_connection(connection);
    free_descriptor(descriptor);
    free_session(session);
    
    printf("We're done\n");
}

}
} // ns grpc
} // ns auxl
