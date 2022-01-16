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

void session_did_close(MSessionResponse* response) {
    if (response->status_code == MSESSION_STATUS_CODE_OK) {
        printf("The session did indeed close with OK status\n");
        
        printf("Metadata:\n");
        for (int i = 0; i < response->metadata->count; i++) {
            printf("Key: %s, val: %s\n", response->metadata->keys[i], response->metadata->vals[i]);
        }
    } else {
        printf("The session closed with an error: %s", response->response);
    }
    free_session_response(response);
}


TEST_F(AuxlExternTest, TestCInterface)
{
    GRPCConnectionOptions def = init_connection_options();
    MConnectionHandle connection = create_connection((char*) "localhost:5000", def);
    
    MDescriptorHandle descriptor = create_descriptor(NULL, 0, connection);
    
    MSessionDelegate delegate = {
        .output_type_name = (char*) "greet.HelloReply",
        .session_did_start = &session_did_start,
        .session_did_send = &session_did_send,
        .session_did_recieve = &session_did_recieve,
        .session_did_close = &session_did_close
    };
    
    MSessionHandle session = create_session(connection, descriptor, &delegate);
    
    char* json_message = (char*) "{ \"name\":\"Extern test\" }";
    MMessageHandle message = descriptor_create_message_from_json(descriptor, json_message, (char*) "greet.HelloRequest");
    
    if (session_start(session, descriptor, (char*) "greet.Greeter.SayHello") != -1) {
        session_send_message(session, message);
        session_close(session);
    }
    
    free_connection(connection);
    free_descriptor(descriptor);
    free_session(session);
}

}
} // ns grpc
} // ns auxl
