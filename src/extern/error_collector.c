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

#include <stdlib.h>
#include <string.h>
#include "error_collector.h"

#define MAX_ERROR_SIZE 32

/**
 */
AuxlGRPCErrorCollector* create_error_collector(void)
{
    AuxlGRPCErrorCollector* collector = (struct AuxlGRPCErrorCollector*) malloc(sizeof(AuxlGRPCErrorCollector));
    
    collector->error_count = 0;
    collector->errors = (AuxlGRPCError**) malloc(sizeof(AuxlGRPCError*) * MAX_ERROR_SIZE);
        
    return collector;
}

/**
 private
 */
int error_collector_add_error_(AuxlGRPCErrorCollector* collector, AuxlGRPCError error)
{
    if (collector->error_count + 1 > MAX_ERROR_SIZE) {
        return -1;
    }
    
    AuxlGRPCError *err = (AuxlGRPCError *) malloc(sizeof(AuxlGRPCError));
    memcpy(err, &error, sizeof(AuxlGRPCError));
    
    collector->errors[collector->error_count] = err;
    collector->error_count++;
    
    return collector->error_count;
}

/**
 */
int error_collector_add_error(AuxlGRPCErrorCollector* collector,
                              AuxlGRPCErrorType type,
                              char *message,
                              AuxlGRPCErrorLevel level)
{
    AuxlGRPCError err = {
        .message = strdup(message),
        .type = type,
        .level = level
    };
    
    return error_collector_add_error_(collector, err);
}

/**
 */
bool error_collector_has_fatal_error(const AuxlGRPCErrorCollector* collector)
{
    for (int i = 0; i < collector->error_count; i++) {
        if (collector->errors[i]->level == FATAL) {
            return true;
        }
    }
    return false;
}

/**
 */
void error_collector_free(AuxlGRPCErrorCollector* collector)
{
    if (collector == NULL) {
        return;
    }
    
    for (int i = 0; i < collector->error_count; i++) {
        free(collector->errors[i]->message);
        free(collector->errors[i]);
    }
    
    free(collector->errors);
    free(collector);
    
    collector = NULL;
}

/**
 */
AuxlGRPCErrorCollector* error_collector_copy(AuxlGRPCErrorCollector* collector)
{
    if (collector == NULL) {
        return NULL;
    }
    
    AuxlGRPCErrorCollector *new_collector = create_error_collector();
    
    for (int i = 0; i < collector->error_count; i++) {
        error_collector_add_error_(new_collector, *collector->errors[i]);
    }
    return new_collector;
}
