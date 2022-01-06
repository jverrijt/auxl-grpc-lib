
#ifndef AUXL_GRPC_TYPES__
#define AUXL_GRPC_TYPES__

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    Unary,
    ClientStreaming,
    ServerStreaming,
    BidirectionalStreaming
} GRPCCallType;

typedef struct GRPCCallInfo_t {
    GRPCCallType type;
    char* name;
} GRPCCallInfo;

#ifdef __cplusplus
}
#endif

#endif
