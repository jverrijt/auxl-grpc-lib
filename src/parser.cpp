
#include "parser.h"
#include <google/protobuf/compiler/importer.h>

#include <google/protobuf/util/json_util.h>
#include "../modules/grpc/test/cpp/util/proto_reflection_descriptor_database.h"

#include <iostream>
#include <libgen.h>

#include <nlohmann/json.hpp>

using namespace google::protobuf;
using namespace nlohmann;

using namespace ::grpc;

namespace auxl {
namespace grpc {

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
    
    compiler::Importer importer(&source_tree, nullptr /*, &error_collector*/);

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
void descriptors_from_reflect(Connection& connection,
                              google::protobuf::SimpleDescriptorDatabase* db,  AuxlGRPCErrorCollector *error_collector)
{
    auto desc_db = std::shared_ptr<ProtoReflectionDescriptorDatabase>(
        new ProtoReflectionDescriptorDatabase(connection.channel));
    
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
}

/**
 */
std::shared_ptr<google::protobuf::DescriptorDatabase> parse_descriptors(std::string descriptors)
{
    json o = json::parse(descriptors);
    auto descr_db = std::make_shared<SimpleDescriptorDatabase>();
    
    for (int i = 0; i < o.size(); i++) {
        auto proto = new google::protobuf::FileDescriptorProto();
        util::JsonStringToMessage(o[i].dump(-1), proto);
        descr_db->Add(*proto);
        
        delete proto;
    }
    
    return descr_db;
}
}
}

