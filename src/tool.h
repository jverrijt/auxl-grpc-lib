//
//  tool.h
//  auxl-grpc
//
//  Created by Joost Verrijt on 15/01/2022.
//

#ifndef tool_h
#define tool_h

#include "options.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef void* MSessionHandle;
typedef void* MMmethodHandle;
typedef void* MDescriptorHandle;
typedef void* MConnectionHandle;

// Connection
MConnectionHandle create_connection(char* endpoint, GRPCConnectionOptions opts);
void free_connection(MConnectionHandle connection);

// Descriptor
MDescriptorHandle create_descriptor(char** protof_files, int proto_file_count, MConnectionHandle connection);
char* descriptor_to_json(MDescriptorHandle handle);

/**
 */
MMmethodHandle get_method_handle(MDescriptorHandle descriptor, char* method_name);

void free_descriptor(MDescriptorHandle descriptor);

// Session
MSessionHandle create_session(MConnectionHandle connection);
void session_start(MSessionHandle session, MMmethodHandle method);

void free_session(MSessionHandle handle);



#ifdef __cplusplus
}
#endif

#endif /* tool_h */
