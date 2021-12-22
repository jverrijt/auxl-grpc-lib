
#ifndef AUXL_GRPC_TYPES__
#define AUXL_GRPC_TYPES__

#ifdef __cplusplus
extern "C" {


typedef enum GRPCConnectionError_t {
    CONNECTION_ERROR = 0,
    COULD_NOT_OPEN_SSL_CERT = 1
} GRPCConnectionError;


typedef enum ParserErrorType_t {
    warning, error
} ParserErrorType;

typedef struct ParserErrorEntry_t {
    ParserErrorType error_type;
    char* filename;
    char* message;
} ParserErrorEntry;


typedef struct GRPCCallStatus {
    int code;
    char *message;
} GRPCCallStatus;


typedef enum kGRPCFormatType {
    JSON, binary, text
} kGRPCFormatType;

typedef enum kGRPCCallType {
    GRPCCallTypeUndefined, 
    GRPCCallTypeList, 
    CRPCCallTypeType, 
    GRPCCallTypeCall
} kGRPCCallType;

/**
 */
typedef struct GRPCOptions {
    bool use_ssl;
    bool use_remote_db;
    bool print_type_as_json;
    
    char *grpc_service_config;
    char *ssl_client_cert;
    char *ssl_client_key;
    
    kGRPCFormatType input_type;
    kGRPCFormatType output_type;
    
} GRPCOptions;


typedef struct GRPCConfig {
    
    kGRPCCallType call_type;
    
    char* endpoint;
    char* type;
    char* service_method;
    char* proto_path;
    char* proto_files;
    char* input_file;
    char* output_file;
    
    GRPCOptions options;
        
    pthread_t thread_id;
    
} GRPCConfig;




}

#endif

#endif
