//
//  options.h
//  auxl-grpc
//
//  Created by Joost Verrijt on 04/01/2022.
//

#ifndef options_h
#define options_h

#include <stdio.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 */
typedef struct GRPCConnectionOptions {
    double timeout;
    bool use_ssl;
    
    char* ssl_client_cert;
    char* ssl_client_key;
    char* ssl_root_certs_path;
    
} GRPCConnectionOptions;

/**
 */
GRPCConnectionOptions init_connection_options(void);

/**
 */
GRPCConnectionOptions* connection_options_create(double timeout,
                                                 bool use_ssl,
                                                 char* ssl_client_cert,
                                                 char* ssl_client_key,
                                                 char* ssl_root_certs_path);

/**
 */
GRPCConnectionOptions* connection_options_default(void);

/**
 */
void connection_options_free(GRPCConnectionOptions* opts);

#ifdef __cplusplus
}
#endif

#endif /* options_h */
