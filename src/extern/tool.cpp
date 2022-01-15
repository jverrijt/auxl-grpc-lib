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

#include "tool.h"

#include "connection.hpp"
#include "descriptor.hpp"
#include "session.hpp"

#include "extern_session_delegate.hpp"

using namespace auxl::grpc;

extern "C"
{
    /**
     */
    MConnectionHandle create_connection(char* endpoint, GRPCConnectionOptions opts) {
        auto connection = Connection::create_connection(endpoint, opts);
        MConnectionHandle mc = (MConnectionHandle) connection.release();
        return mc;
    }
    
    /**
     */
    void free_connection(MConnectionHandle connection) {
        Connection* c = (Connection*) connection;
        delete(c);
    }
    
    /**
     */
    MDescriptorHandle create_descriptor(char** proto_files, int proto_file_count, MConnectionHandle connection) {
        std::vector<std::string> v_proto;
            
        for(int i = 0; i < proto_file_count; i++) {
            v_proto.push_back(proto_files[i]);
        }
            
        auto descriptor = new Descriptor(v_proto, (Connection *) connection);
        return descriptor;
    }
    
    /**
     */
    char* descriptor_to_json(MDescriptorHandle handle) {
        Descriptor *d = (Descriptor*) handle;
        std::string json = d->to_json();
        return strdup(json.c_str());
    }
    
    /**
     */
    void free_descriptor(MDescriptorHandle descriptor) {
        Descriptor *d = (Descriptor*) descriptor;
        delete(d);
    }
    
    // MARK: session
    /**
     */
    MSessionHandle create_session(MConnectionHandle connection, MDescriptorHandle descriptor, MSessionDelegate* delegate) {
        auto session = new Session((Connection *) connection);
        
        if (delegate != NULL) {
            auto d = new ExternSessionDelegate((Descriptor*) descriptor, delegate);
            session->delegate = d;
        }
        
        return (MSessionHandle*) session;
    }
    
    /**
     */
    MSessionResponse* create_session_response(char* response, MMetadata* metadata) {
        MSessionResponse* r = (MSessionResponse*) malloc(sizeof(MSessionResponse));
        r->response = strdup(response);
        metadata = metadata;
        return r;
    }
    
    /**
     */
    MSessionDelegate* create_session_delegate(char* output_type_name, session_did_start_m start_method, session_did_send_m send_method, session_did_receive_m receive_method, session_did_close_m close_method) {
        
        MSessionDelegate* delegate = (MSessionDelegate*) malloc(sizeof(MSessionDelegate));
        
        delegate->output_type_name = strdup(output_type_name);
        delegate->session_did_start = start_method;
        delegate->session_did_send = send_method;
        delegate->session_did_receive = receive_method;
        delegate->session_did_close = close_method;
        
        return delegate;
    }
    
    /**
     */
    void free_session_delegate(MSessionDelegate* delegate) {
        free(delegate->output_type_name);
        free(delegate);
    }
    
    /**
     */
    int session_start(MSessionHandle session, MDescriptorHandle descriptor, char* method_name) {
        const auto method = ((Descriptor*) descriptor)->get_method_descriptor(method_name);
        
        if (method == nullptr) {
            return -1;
        }
        
        if (((Session*) session)->start(*method)) {
            return 0;
        }
        
        return -1;
    }

    /**
     */
    void free_session(MSessionHandle handle) {
        delete((Session*) handle);
    }
    
    /**
     */
    void free_session_response(MSessionResponse* response) {
        free(response->response);
        free_metadata(response->metadata);
        free(response);
    }
    
    // MARK: metadata
    
    /**
     */
    MMetadata *create_metadata(size_t size) {
        MMetadata* data = (MMetadata*) malloc(sizeof(MMetadata));
        data->keys = (char**) malloc(sizeof(char*) * size);
        data->vals = (char**) malloc(sizeof(char*) * size);
        
        data->size = size;
        data->count = 0;
        
        return data;
    }
    
    /**
     */
    void metadata_push(MMetadata* data, const char* key, const char* val) {
        if (data->count + 1 < data->size) {
            data->keys[data->count] = strdup(key);
            data->vals[data->count] = strdup(val);
            data->count++;
        }
    }

    /**
     */
    void free_metadata(MMetadata *data) {
        for (int i = 0; i < data->count; i++) {
            free(data->keys[i]);
            free(data->vals[i]);
        }
        
        free(data->keys);
        free(data->vals);
        free(data);
    }
}
