
#include "tool.h"

#include "connection.hpp"
#include "descriptor.hpp"
#include "session.hpp"

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
    MMmethodHandle get_method_handle(MDescriptorHandle descriptor, char* method_name) {
        Descriptor *d = (Descriptor*) descriptor;
        
        // Descriptor owns method handles.
        return (MMmethodHandle) d->get_method_descriptor(method_name);
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
    
    /**
     */
    MSessionHandle create_session(MConnectionHandle connection) {
        auto session = new Session((Connection *) connection);
        return (MSessionHandle*) session;
    }
    
    void session_start(MSessionHandle session, MMmethodHandle method) {
        
    }

    void free_session(MSessionHandle handle) {
        delete((Session*) handle);
    }
    
}
