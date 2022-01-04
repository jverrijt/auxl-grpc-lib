//
//  options.c
//  auxl-grpc
//
//  Created by Joost Verrijt on 04/01/2022.
//

#include "options.h"
#include <string.h>
#include <stdlib.h>

/**
 */
GRPCConnectionOptions* connection_options_create(bool use_ssl,
                                      char* ssl_client_cert,
                                      char* ssl_client_key,
                                      char* ssl_root_certs_path)
{
    GRPCConnectionOptions* opts = (GRPCConnectionOptions*) malloc(sizeof(GRPCConnectionOptions));
        
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
    return  connection_options_create(false, "", "", "");
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
