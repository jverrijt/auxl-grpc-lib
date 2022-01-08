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
