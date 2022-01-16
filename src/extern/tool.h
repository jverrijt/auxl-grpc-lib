/*
 *
 * Copyright 2022 Joost Verrijt.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef tool_h
#define tool_h

#include "options.h"
#include "error_collector.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MSESSION_STATUS_CODE_ERROR -1
#define MSESSION_STATUS_CODE_OK 0

typedef void* MSessionHandle;
typedef void* MMessageHandle;
typedef void* MDescriptorHandle;
typedef void* MConnectionHandle;

typedef struct MMetadata_t {
    size_t size;
    int count;
    char** keys;
    char** vals;
} MMetadata;

typedef struct MSessionResponse_t {
    int status_code;
    char* response;
    MMetadata* metadata;
} MSessionResponse;

// Function pointers
typedef void (*session_did_start_m)(void);
typedef void (*session_did_send_m)(MMessageHandle message);
typedef void (*session_did_recieve_m)(MSessionResponse* response);
typedef void (*session_did_close_m)(MSessionResponse* status, MMetadata* closing_metadata);

typedef struct MSessionDelegate_t {
    char* output_type_name;
    session_did_start_m session_did_start;
    session_did_send_m session_did_send;
    session_did_recieve_m session_did_recieve;
    session_did_close_m session_did_close;
} MSessionDelegate;

/* Connection */
MConnectionHandle create_connection(char* endpoint, GRPCConnectionOptions opts);
void free_connection(MConnectionHandle connection);

/* Descriptor */
MDescriptorHandle create_descriptor(char** protof_files, int proto_file_count, MConnectionHandle connection);

/**
 Returns the error collector for a given descriptor handle
 */
const AuxlGRPCErrorCollector* descriptor_get_errors(MDescriptorHandle handle);
char* descriptor_to_json(MDescriptorHandle handle);

MMessageHandle descriptor_create_message_from_json(MDescriptorHandle handle, char* json, char* message_type);
char* descriptor_create_json_message(MDescriptorHandle handle, char* type_name);

void free_descriptor(MDescriptorHandle descriptor);

/* Session */
MSessionHandle create_session(MConnectionHandle connection, MDescriptorHandle descriptor, MSessionDelegate *delegate);
MSessionResponse* create_session_response(char* response, MMetadata* metadata);

int session_start(MSessionHandle session, MDescriptorHandle descriptor, char* method_name);

void session_send_message(MSessionHandle handle, char* json, char* message_type_name);

void session_close(void);

void free_session(MSessionHandle handle);
void free_session_response(MSessionResponse* response);

/* Metadata */
MMetadata *create_metadata(size_t size);
void metadata_push(MMetadata* data, const char* key, const char* val);
void free_metadata(MMetadata* data);

#ifdef __cplusplus
}
#endif

#endif /* tool_h */
