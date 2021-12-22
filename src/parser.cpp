
#include "parser.h"

#include <google/protobuf/compiler/importer.h>

#include <iostream>
#include <libgen.h>

using namespace google::protobuf::compiler;


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


/**
*/
std::string proto_files_to_fd_json(std::vector<std::string> proto_files) {
    
    auxl::grpc::ProtoFileParserError error_collector;

    DiskSourceTree source_tree;

    // Get the base paths and add them to the source tree
    for (std::string file : proto_files) {
        char* path = dirname((char*) file.c_str());
        // grpc::protobuf::File
        source_tree.MapPath("", path);
    }
    
    Importer importer(&source_tree, &error_collector);

    for (std::string file : proto_files) {
        char *name = basename((char *) file.c_str());
        printf("Basename: %s\n", name);
        const google::protobuf::FileDescriptor *descr = importer.Import(name);

        auto proto = new google::protobuf::FileDescriptorProto();
        descr->CopyTo(proto);


        // for (int i = 0; i < descr->dependency_count(); i++) {
        //     descr->dependency(i)

        // }

        std::cout << proto->DebugString();

        delete proto;
    }



    // Importer importer(&source_tree, &error_collector);
    


    // auto pool = importer.pool();

    // std::cout << pool->FindMethodByName("echo")->DebugString();
    

    

    // parser.Parse()

    throw AuxlGRPCParserException((char*) "Bad file", AuxlGRPCParserError::CouldNotFindFile);

    return NULL;
}



}
}

