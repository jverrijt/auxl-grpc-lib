//
//  error_collector.c
//  auxl-grpc
//
//  Created by Joost Verrijt on 02/01/2022.
//

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
 */
int error_collector_add_error_type_message(AuxlGRPCErrorCollector* collector, AuxlGRPCErrorType type, char *message, AuxlGRPCErrorLevel level)
{
    AuxlGRPCError err = {
        .message = message,
        .type = type,
        .level = level
    };
    
    return error_collector_add_error(collector, err);
}

/**
 */
int error_collector_add_error(AuxlGRPCErrorCollector* collector, AuxlGRPCError error)
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
bool error_collector_has_fatal_error(AuxlGRPCErrorCollector* collector)
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
        error_collector_add_error(new_collector, *collector->errors[i]);
    }
    return new_collector;
}
