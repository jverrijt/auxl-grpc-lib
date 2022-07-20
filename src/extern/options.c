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
        .enable_tls = false,
        .validate_tls = true
    };
    
    return opts;
}

/**
 */
GRPCConnectionOptions* connection_options_create(double timeout,
                                                 bool enable_tls,
                                                 bool validate_tls,
                                                 char* pem_root_certs_path)
{
    GRPCConnectionOptions* opts = (GRPCConnectionOptions*) malloc(sizeof(GRPCConnectionOptions));
    
    opts->timeout = timeout;
    opts->enable_tls = enable_tls;
    opts->validate_tls = validate_tls;
    opts->pem_root_certs_path = strdup(pem_root_certs_path);

    return opts;
}

/**
 */
GRPCConnectionOptions* connection_options_default()
{
    return  connection_options_create(-1, false, true, "");
}

/**
 */
void connection_options_free(GRPCConnectionOptions* opts)
{
    free(opts->pem_root_certs_path);
    free(opts);
}
