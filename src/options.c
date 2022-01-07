//
//  options.c
//  auxl-grpc
//
//  Created by Joost Verrijt on 04/01/2022.
//

#include "options.h"
#include <string.h>
#include <stdlib.h>


GRPCConnectionOptions init_connection_options()
{
    GRPCConnectionOptions opts = {
        .timeout = -1,
        .use_ssl = false
    };
    
    return opts;
}

/**
 */
GRPCConnectionOptions* connection_options_create(double timeout,
                                                 bool use_ssl,
                                                 char* ssl_client_cert,
                                                 char* ssl_client_key,
                                                 char* ssl_root_certs_path)
{
    GRPCConnectionOptions* opts = (GRPCConnectionOptions*) malloc(sizeof(GRPCConnectionOptions));
    
    opts->timeout = timeout;
    opts->use_ssl = use_ssl;
    
    opts->ssl_client_cert = strdup(ssl_client_cert);
    opts->ssl_client_key = strdup(ssl_client_key);
    opts->ssl_root_certs_path = strdup(ssl_root_certs_path);

    return opts;
}

/**
 */
GRPCConnectionOptions* connection_options_default()
{
    return  connection_options_create(-1, false, "", "", "");
}

/**
 */
void connection_options_free(GRPCConnectionOptions* opts)
{
    free(opts->ssl_client_cert);
    free(opts->ssl_client_key);
    free(opts->ssl_root_certs_path);
    
    free(opts);
}
