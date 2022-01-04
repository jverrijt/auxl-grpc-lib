
#ifndef AUXL_GRPC_TYPES__
#define AUXL_GRPC_TYPES__

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif

#endif
