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
