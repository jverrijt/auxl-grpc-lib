
#include "parser.h"
#include <google/protobuf/compiler/importer.h>

#include <google/protobuf/util/json_util.h>
#include "../modules/grpc/test/cpp/util/proto_reflection_descriptor_database.h"

#include <iostream>
#include <libgen.h>

using namespace google::protobuf::compiler;

using google::protobuf::DescriptorPoolDatabase;

using namespace ::grpc;

namespace auxl {
namespace grpc {

/**
*/
void ProtoFileParserError::AddError(const std::string &filename, int line, int column, const std::string &message) {

    ParserErrorEntry entry = { 
        .error_type = warning, 
        .filename = (char*) filename.c_str(), 
        .message = (char*) message.c_str() 
    };

    entries.push_back(entry);
    std::cerr << "Error in filename " << filename << " Message:" << message << "\n";
}

/**
 */
void ProtoFileParserError::AddWarning(const std::string &filename, int line, int column, const std::string &message) {
    ParserErrorEntry entry = { 
        .error_type = warning, 
        .filename = (char*) filename.c_str(), 
        .message = (char*) message.c_str() 
    };

    entries.push_back(entry);
    std::cerr << "Warning in filename " << filename;
}

/**
 */
void descriptor_db_from_proto_files(
    std::vector<std::string> proto_files, google::protobuf::SimpleDescriptorDatabase* db) {

    auxl::grpc::ProtoFileParserError error_collector;
    DiskSourceTree source_tree;

    // Get the base paths and add them to the source tree
    for (std::string file : proto_files) {
        char* path = dirname((char*) file.c_str());
        source_tree.MapPath("", path);
    }
    
    Importer importer(&source_tree, &error_collector);

    // Collect the proto file names in these proto files
    for (std::string file : proto_files) {
        char *name = basename((char *) file.c_str());
        const google::protobuf::FileDescriptor *descr = importer.Import(name);

        if (descr != nullptr) {
            auto proto = new google::protobuf::FileDescriptorProto();
            descr->CopyTo(proto);

            db->Add(*proto);

            // Collect dependencies
            for (int i = 0; i < proto->dependency_size(); i++) {
                auto dep = proto->dependency(i);
                auto dep_descr = importer.pool()->FindFileByName(dep);
                
                auto dep_proto = new google::protobuf::FileDescriptorProto();
                dep_descr->CopyTo(dep_proto);
                db->Add(*dep_proto);

                delete dep_proto;
            }

            delete proto;
        }
    }
}

/**
 * 
 */
void grpc_reflect(std::shared_ptr<Connection>& connection, google::protobuf::SimpleDescriptorDatabase* db) 
{
    auto desc_db = std::shared_ptr<ProtoReflectionDescriptorDatabase>(
        new ProtoReflectionDescriptorDatabase(connection->channel));
    google::protobuf::DescriptorPool desc_pool(desc_db.get());

    desc_pool.AllowUnknownDependencies();
    
    std::vector<std::string> service_names;
    std::vector<std::string> file_names;
    
    desc_db->GetServices(&service_names);
    
    for (std::string serv : service_names) {
        std::cout << serv << std::endl;

        auto service = desc_pool.FindServiceByName(serv);
        std::cout << "Service Filename: " << service->file()->name() << std::endl;

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
 * 
 */
std::string describe(std::vector<std::string> proto_files, std::shared_ptr<Connection>* connection) 
{
    google::protobuf::SimpleDescriptorDatabase descr_db;

    if (proto_files.size() > 0) {
        descriptor_db_from_proto_files(proto_files, &descr_db);
    }

    if (connection != NULL) {
        grpc_reflect(*connection, &descr_db);
    }

    std::vector<std::string> file_names;
    descr_db.FindAllFileNames(&file_names);

    google::protobuf::util::JsonPrintOptions jsonPrintOptions;

    jsonPrintOptions.add_whitespace = true;
    jsonPrintOptions.always_print_primitive_fields = true;
    jsonPrintOptions.always_print_enums_as_ints = true;

    std::string jsonOutput;
    jsonOutput += "[";

    for (std::string fn : file_names) {
        const auto proto = new google::protobuf::FileDescriptorProto();

        if (descr_db.FindFileByName(fn, proto)) {
            std::string output;
            google::protobuf::util::MessageToJsonString(*proto, &output);
            jsonOutput += output + ",";            
        }
    }

    // Remove trailing comma
    if (!jsonOutput.empty()) {
        jsonOutput.pop_back();
    }

    jsonOutput += "]";

    return jsonOutput;
}

}
}

