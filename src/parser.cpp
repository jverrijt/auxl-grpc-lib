
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

    std::cout << "Error in filename " << filename << " Message:" << message << "\n";
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
    std::cout << "Warning in filename " << filename;
}


std::shared_ptr<Importer> cur_importer;

/**
 */
std::shared_ptr<DescriptorPoolDatabase> descriptor_db_from_proto_files(
    std::vector<std::string> proto_files, std::set<std::string>* proto_file_names) {

    auxl::grpc::ProtoFileParserError error_collector;
    DiskSourceTree source_tree;

    // Get the base paths and add them to the source tree
    for (std::string file : proto_files) {
        char* path = dirname((char*) file.c_str());
        source_tree.MapPath("", path);
    }
    
    // Hold on to the importer as when it goes out-of-scope, so will its pool.
    cur_importer = std::shared_ptr<Importer>(new Importer(&source_tree, &error_collector));

    // Collect the proto file names in these proto files
    for (std::string file : proto_files) {
        char *name = basename((char *) file.c_str());
        const google::protobuf::FileDescriptor *descr = cur_importer->Import(name);

        if (descr != nullptr) {
            const auto proto = new google::protobuf::FileDescriptorProto();
            descr->CopyTo(proto);
            proto_file_names->insert(proto->name());

            // Also insert its dependencies
            for (int i = 0; i < proto->dependency_size(); i++) {
                auto dep = proto->dependency(i);
                proto_file_names->insert(dep);
            }

            delete proto;
        }
    }

    auto n = std::shared_ptr<DescriptorPoolDatabase>(
        new DescriptorPoolDatabase(*cur_importer->pool()));

    return n;
}

/**
*/
std::string proto_files_to_fd_json(std::vector<std::string> proto_files) {

    std::set<std::string> available_file_names;
    auto descr_db = descriptor_db_from_proto_files(proto_files, &available_file_names);

    std::set<std::string>::iterator it = available_file_names.begin();

    std::cout << "av c: " << available_file_names.size() << std::endl;

    // Iterate till the end of set
    while (it != available_file_names.end()) {
        const auto proto = new google::protobuf::FileDescriptorProto();
        std::cout << "it: " + *it << std::endl;
        if (descr_db->FindFileByName(*it, proto)) {
            std::cout << proto->DebugString() << std::endl;
        }

        it++;

        delete proto;
    }

    cur_importer.reset();

    return "";
}


std::string grpc_reflect(std::shared_ptr<Connection>& connection) {

    ProtoReflectionDescriptorDatabase desc_db(connection->channel);
    google::protobuf::DescriptorPool desc_pool(&desc_db);
    
    desc_pool.AllowUnknownDependencies();
    
    std::string jsonOutput;

    google::protobuf::util::JsonPrintOptions jsonPrintOptions;

    jsonPrintOptions.add_whitespace = true;
    jsonPrintOptions.always_print_primitive_fields = true;
    jsonPrintOptions.always_print_enums_as_ints = true;
    
    std::vector<std::string> service_names;
    std::vector<std::string> file_names;
    


    desc_db.GetServices(&service_names);
    
    for (std::string serv : service_names) {
        std::cout << serv << std::endl;

        auto service = desc_pool.FindServiceByName(serv);

        std::cout << "Service Filename: " << service->file()->name() << std::endl;
    }

    return "";
}


}
}

