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

#ifndef error_collector_h
#define error_collector_h

#include <stdio.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum AuxlGRPCErrorType
{
    PROTO_ERROR = 1,
    REFLECTION_NOT_AVAILABLE = 2,
    REFLECTION_FAILED = 3,
    ENDPOINT_NOT_REACHEABLE = 4
} AuxlGRPCErrorType;


typedef enum AuxlGRPCErrorLevel
{
    WARNING,
    FATAL
} AuxlGRPCErrorLevel;

typedef struct AuxlGRPCError
{
    char* message;
    AuxlGRPCErrorType type;
    AuxlGRPCErrorLevel level;
    
} AuxlGRPCError;

typedef struct AuxlGRPCErrorCollector
{
    int error_count;
    AuxlGRPCError** errors;
} AuxlGRPCErrorCollector;

/**
 */
AuxlGRPCErrorCollector* create_error_collector(void);

/**
 */
int error_collector_add_error(AuxlGRPCErrorCollector* collector,
                              AuxlGRPCErrorType type,
                              char *message,
                              AuxlGRPCErrorLevel level);

/**
 */
bool error_collector_has_fatal_error(const AuxlGRPCErrorCollector* collector);

/**
 Returns a deep-copy of the given error collector that you'll need to free.
 */
AuxlGRPCErrorCollector* error_collector_copy(AuxlGRPCErrorCollector* collector);

/**
 */
void error_collector_free(AuxlGRPCErrorCollector* collector);

#ifdef __cplusplus
}
#endif

#endif /* error_collector_h */
