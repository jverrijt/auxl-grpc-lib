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
    MConnectionHandle create_connection(const char* endpoint, GRPCConnectionOptions opts) {
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
    const AuxlGRPCErrorCollector* descriptor_get_errors(MDescriptorHandle handle) {
        return ((Descriptor*) handle)->get_error_collector();
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
    MMessageHandle descriptor_create_message_from_json(MDescriptorHandle handle, const char* json, const char* message_type) {
        Descriptor *d = (Descriptor*) handle;
        auto msg = d->message_from_json(message_type, json);
        MMessageHandle mc = (MMessageHandle) msg.release();
        return mc;
    }
    
    /**
     */
    char* descriptor_create_json_message(MDescriptorHandle handle, const char* type_name) {
        Descriptor* descriptor = (Descriptor*) handle;
        auto msg = descriptor->create_message(type_name);
        
        if (msg != nullptr) {
            return strdup(descriptor->message_to_json(*msg).c_str());
        }
        
        return NULL;
    }
    
    /**
     */
    void free_descriptor(MDescriptorHandle handle) {
        delete((Descriptor*) handle);
    }
    
    /**
     */
    void free_message(MMessageHandle handle) {
        delete((google::protobuf::Message*) handle);
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
    MSessionResponse* create_session_response(const char* response, MMetadata* metadata, int status_code) {
        MSessionResponse* r = (MSessionResponse*) malloc(sizeof(MSessionResponse));
        r->response = strdup(response);
        r->metadata = metadata;
        r->status_code = status_code;
        return r;
    }
    
    /**
     */
    int session_start(MSessionHandle session, MDescriptorHandle descriptor, const char* method_name) {
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
    void session_send_message(MSessionHandle handle, MMessageHandle message) {
        auto msg = (google::protobuf::Message*) message;
        ((Session*) handle)->send_message(*msg);
    }
    
    /**
     */
    void session_close(MSessionHandle handle) {
        ((Session*) handle)->close();
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
    MMetadata* create_metadata(size_t size) {
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
        if (data->count + 1 <= data->size) {
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
