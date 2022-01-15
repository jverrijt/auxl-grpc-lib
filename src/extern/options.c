/*
 *
 * Copyright 2022 Joost Verrijt.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
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
