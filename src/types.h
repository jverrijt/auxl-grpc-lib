
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
typedef struct GRPCConnectionOptions {
    bool use_ssl;
    
    char* ssl_client_cert;
    char* ssl_client_key;
    char* ssl_root_certs_path;
    
} GRPCConnectionOptions;


}

#endif

#endif
