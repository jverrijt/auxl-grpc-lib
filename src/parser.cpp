
#include "parser.h"
#include <google/protobuf/compiler/importer.h>

#include <google/protobuf/util/json_util.h>
#include "proto_reflection_descriptor_database.h"

#include <iostream>
#include <libgen.h>
#include <sstream>

#include <nlohmann/json.hpp>

using namespace google::protobuf;
using namespace nlohmann;

using namespace ::grpc;

namespace auxl {
namespace grpc {


class ParserErrorCollector : public compiler::MultiFileErrorCollector {
public:
    explicit ParserErrorCollector(AuxlGRPCErrorCollector* collector) : collector_(collector) {}
    
    void AddError(const std::string& filename, int line, int column,
                  const std::string& message) override {
        std::string full_message = message + " -> " + filename;
        error_collector_add_error(collector_, PROTO_ERROR, (char*) full_message.c_str(), FATAL);
    }
    
    void AddWarning(const std::string& filename, int line, int column,
                    const std::string& message) override {
        std::string full_message = message + " -> " + filename;
        error_collector_add_error(collector_, PROTO_ERROR, (char*) full_message.c_str(), WARNING);
    }
    
private:
    AuxlGRPCErrorCollector* collector_;
};

/**
 */
void descriptors_from_proto_files(std::vector<std::string> proto_files,
                                  google::protobuf::SimpleDescriptorDatabase* db,  AuxlGRPCErrorCollector *error_collector)
{
    compiler::DiskSourceTree source_tree;
    
    // Get the base paths and add them to the source tree
    for (std::string file : proto_files) {
        char* path = dirname((char*) file.c_str());
        source_tree.MapPath("", path);
    }
    
    std::unique_ptr<ParserErrorCollector> p(new ParserErrorCollector(error_collector));
    
    compiler::Importer importer(&source_tree, p.get());
    
    // Collect the proto file names in these proto files
    for (std::string file : proto_files) {
        char *name = basename((char *) file.c_str());
        const google::protobuf::FileDescriptor *descr = importer.Import(name);
        
        if (descr != nullptr) {
            auto proto = new FileDescriptorProto();
            descr->CopyTo(proto);
            
            db->Add(*proto);
            
            // Collect dependencies
            for (int i = 0; i < proto->dependency_size(); i++) {
                auto dep = proto->dependency(i);
                auto dep_descr = importer.pool()->FindFileByName(dep);
                
                auto dep_proto = new FileDescriptorProto();
                dep_descr->CopyTo(dep_proto);
                db->Add(*dep_proto);
                
                delete dep_proto;
            }
            
            delete proto;
        }
    }
}

/**
 */
void descriptors_from_reflect(const Connection& connection, SimpleDescriptorDatabase* db,
                              AuxlGRPCErrorCollector *error_collector)
{
    auto desc_db = std::shared_ptr<ProtoReflectionDescriptorDatabase>(new ProtoReflectionDescriptorDatabase(connection.channel));
    
    DescriptorPool desc_pool(desc_db.get());
    
    desc_pool.AllowUnknownDependencies();
    
    std::vector<std::string> service_names;
    std::vector<std::string> file_names;
    
    desc_db->GetServices(&service_names);
    
    for (std::string serv : service_names) {
        // std::cout << serv << std::endl;
        
        auto service = desc_pool.FindServiceByName(serv);
        // std::cout << "Service Filename: " << service->file()->name() << std::endl;
        
        // Get the service dependencies
        auto fd = new google::protobuf::FileDescriptorProto();
        desc_db->FindFileByName(service->file()->name(), fd);
        
        db->Add(*fd);
        
        // Collect dependencies
        for (int i = 0; i < fd->dependency_size(); i++) {
            auto dep_name = fd->dependency(i);
            
            auto dep_fd = new google::protobuf::FileDescriptorProto();
            desc_db->FindFileByName(dep_name, dep_fd);
            
            db->Add(*dep_fd);
            
            delete dep_fd;
        }
        
        delete fd;
    }
    
    // Process any errors here
    auto status = desc_db->CloseStream();
    
    if (!status.ok()) {
        if (status.error_code() == StatusCode::UNIMPLEMENTED) {
            error_collector_add_error(error_collector, REFLECTION_NOT_AVAILABLE,
                                      (char*) "This endpoint does not implement the reflection service", FATAL);
        } else {
            std::ostringstream code;
            code << static_cast<int>(status.error_code());
            std::string e = "Reflection call failed: " + status.error_message();
            error_collector_add_error(error_collector, REFLECTION_FAILED, (char*) e.c_str(), FATAL);
        }
    }
}

/**
 */
std::unique_ptr<SimpleDescriptorDatabase> parse_descriptors(const std::string& descriptors)
{
    json o = json::parse(descriptors);
    auto descr_db = std::unique_ptr<SimpleDescriptorDatabase>(new SimpleDescriptorDatabase());
    
    for (int i = 0; i < o.size(); i++) {
        auto proto = new google::protobuf::FileDescriptorProto();
        ::util::JsonStringToMessage(o[i].dump(-1), proto);
        descr_db->Add(*proto);
        
        delete proto;
    }
    
    return descr_db;
}

} // ns grpc
} // ns auxl

