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

#include "grpcpp/security/tls_certificate_verifier.h"

#ifndef InsecureCertificateVerifier_h
#define InsecureCertificateVerifier_h

class InsecureCertificateVerifier : public grpc::experimental::ExternalCertificateVerifier {
public:
    bool Verify(grpc::experimental::TlsCustomVerificationCheckRequest* request, std::function<void(grpc::Status)> callback, grpc::Status* sync_status) override {
        *sync_status = grpc::Status(grpc::StatusCode::OK, "");
        return true;
    }
    
    void Cancel(grpc::experimental::TlsCustomVerificationCheckRequest* request) override {
    }
};

#endif /* InsecureCertificateVerifier_h */
