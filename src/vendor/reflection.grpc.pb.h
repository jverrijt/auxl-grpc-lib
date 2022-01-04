// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: src/proto/grpc/reflection/v1alpha/reflection.proto
// Original file comments:
// Copyright 2016 gRPC authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Service exported by server reflection
//
#ifndef GRPC_src_2fproto_2fgrpc_2freflection_2fv1alpha_2freflection_2eproto__INCLUDED
#define GRPC_src_2fproto_2fgrpc_2freflection_2fv1alpha_2freflection_2eproto__INCLUDED

#include "reflection.pb.h"

#include <functional>
#include <grpcpp/impl/codegen/async_generic_service.h>
#include <grpcpp/impl/codegen/async_stream.h>
#include <grpcpp/impl/codegen/async_unary_call.h>
#include <grpcpp/impl/codegen/client_callback.h>
#include <grpcpp/impl/codegen/client_context.h>
#include <grpcpp/impl/codegen/completion_queue.h>
#include <grpcpp/impl/codegen/message_allocator.h>
#include <grpcpp/impl/codegen/method_handler.h>
#include <grpcpp/impl/codegen/proto_utils.h>
#include <grpcpp/impl/codegen/rpc_method.h>
#include <grpcpp/impl/codegen/server_callback.h>
#include <grpcpp/impl/codegen/server_callback_handlers.h>
#include <grpcpp/impl/codegen/server_context.h>
#include <grpcpp/impl/codegen/service_type.h>
#include <grpcpp/impl/codegen/status.h>
#include <grpcpp/impl/codegen/stub_options.h>
#include <grpcpp/impl/codegen/sync_stream.h>

namespace grpc {
namespace reflection {
namespace v1alpha {

class ServerReflection final {
 public:
  static constexpr char const* service_full_name() {
    return "grpc.reflection.v1alpha.ServerReflection";
  }
  class StubInterface {
   public:
    virtual ~StubInterface() {}
    // The reflection service is structured as a bidirectional stream, ensuring
    // all related requests go to a single server.
    std::unique_ptr< ::grpc::ClientReaderWriterInterface< ::grpc::reflection::v1alpha::ServerReflectionRequest, ::grpc::reflection::v1alpha::ServerReflectionResponse>> ServerReflectionInfo(::grpc::ClientContext* context) {
      return std::unique_ptr< ::grpc::ClientReaderWriterInterface< ::grpc::reflection::v1alpha::ServerReflectionRequest, ::grpc::reflection::v1alpha::ServerReflectionResponse>>(ServerReflectionInfoRaw(context));
    }
    std::unique_ptr< ::grpc::ClientAsyncReaderWriterInterface< ::grpc::reflection::v1alpha::ServerReflectionRequest, ::grpc::reflection::v1alpha::ServerReflectionResponse>> AsyncServerReflectionInfo(::grpc::ClientContext* context, ::grpc::CompletionQueue* cq, void* tag) {
      return std::unique_ptr< ::grpc::ClientAsyncReaderWriterInterface< ::grpc::reflection::v1alpha::ServerReflectionRequest, ::grpc::reflection::v1alpha::ServerReflectionResponse>>(AsyncServerReflectionInfoRaw(context, cq, tag));
    }
    std::unique_ptr< ::grpc::ClientAsyncReaderWriterInterface< ::grpc::reflection::v1alpha::ServerReflectionRequest, ::grpc::reflection::v1alpha::ServerReflectionResponse>> PrepareAsyncServerReflectionInfo(::grpc::ClientContext* context, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncReaderWriterInterface< ::grpc::reflection::v1alpha::ServerReflectionRequest, ::grpc::reflection::v1alpha::ServerReflectionResponse>>(PrepareAsyncServerReflectionInfoRaw(context, cq));
    }
    class async_interface {
     public:
      virtual ~async_interface() {}
      // The reflection service is structured as a bidirectional stream, ensuring
      // all related requests go to a single server.
      virtual void ServerReflectionInfo(::grpc::ClientContext* context, ::grpc::ClientBidiReactor< ::grpc::reflection::v1alpha::ServerReflectionRequest,::grpc::reflection::v1alpha::ServerReflectionResponse>* reactor) = 0;
    };
    typedef class async_interface experimental_async_interface;
    virtual class async_interface* async() { return nullptr; }
    class async_interface* experimental_async() { return async(); }
   private:
    virtual ::grpc::ClientReaderWriterInterface< ::grpc::reflection::v1alpha::ServerReflectionRequest, ::grpc::reflection::v1alpha::ServerReflectionResponse>* ServerReflectionInfoRaw(::grpc::ClientContext* context) = 0;
    virtual ::grpc::ClientAsyncReaderWriterInterface< ::grpc::reflection::v1alpha::ServerReflectionRequest, ::grpc::reflection::v1alpha::ServerReflectionResponse>* AsyncServerReflectionInfoRaw(::grpc::ClientContext* context, ::grpc::CompletionQueue* cq, void* tag) = 0;
    virtual ::grpc::ClientAsyncReaderWriterInterface< ::grpc::reflection::v1alpha::ServerReflectionRequest, ::grpc::reflection::v1alpha::ServerReflectionResponse>* PrepareAsyncServerReflectionInfoRaw(::grpc::ClientContext* context, ::grpc::CompletionQueue* cq) = 0;
  };
  class Stub final : public StubInterface {
   public:
    Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options = ::grpc::StubOptions());
    std::unique_ptr< ::grpc::ClientReaderWriter< ::grpc::reflection::v1alpha::ServerReflectionRequest, ::grpc::reflection::v1alpha::ServerReflectionResponse>> ServerReflectionInfo(::grpc::ClientContext* context) {
      return std::unique_ptr< ::grpc::ClientReaderWriter< ::grpc::reflection::v1alpha::ServerReflectionRequest, ::grpc::reflection::v1alpha::ServerReflectionResponse>>(ServerReflectionInfoRaw(context));
    }
    std::unique_ptr<  ::grpc::ClientAsyncReaderWriter< ::grpc::reflection::v1alpha::ServerReflectionRequest, ::grpc::reflection::v1alpha::ServerReflectionResponse>> AsyncServerReflectionInfo(::grpc::ClientContext* context, ::grpc::CompletionQueue* cq, void* tag) {
      return std::unique_ptr< ::grpc::ClientAsyncReaderWriter< ::grpc::reflection::v1alpha::ServerReflectionRequest, ::grpc::reflection::v1alpha::ServerReflectionResponse>>(AsyncServerReflectionInfoRaw(context, cq, tag));
    }
    std::unique_ptr<  ::grpc::ClientAsyncReaderWriter< ::grpc::reflection::v1alpha::ServerReflectionRequest, ::grpc::reflection::v1alpha::ServerReflectionResponse>> PrepareAsyncServerReflectionInfo(::grpc::ClientContext* context, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncReaderWriter< ::grpc::reflection::v1alpha::ServerReflectionRequest, ::grpc::reflection::v1alpha::ServerReflectionResponse>>(PrepareAsyncServerReflectionInfoRaw(context, cq));
    }
    class async final :
      public StubInterface::async_interface {
     public:
      void ServerReflectionInfo(::grpc::ClientContext* context, ::grpc::ClientBidiReactor< ::grpc::reflection::v1alpha::ServerReflectionRequest,::grpc::reflection::v1alpha::ServerReflectionResponse>* reactor) override;
     private:
      friend class Stub;
      explicit async(Stub* stub): stub_(stub) { }
      Stub* stub() { return stub_; }
      Stub* stub_;
    };
    class async* async() override { return &async_stub_; }

   private:
    std::shared_ptr< ::grpc::ChannelInterface> channel_;
    class async async_stub_{this};
    ::grpc::ClientReaderWriter< ::grpc::reflection::v1alpha::ServerReflectionRequest, ::grpc::reflection::v1alpha::ServerReflectionResponse>* ServerReflectionInfoRaw(::grpc::ClientContext* context) override;
    ::grpc::ClientAsyncReaderWriter< ::grpc::reflection::v1alpha::ServerReflectionRequest, ::grpc::reflection::v1alpha::ServerReflectionResponse>* AsyncServerReflectionInfoRaw(::grpc::ClientContext* context, ::grpc::CompletionQueue* cq, void* tag) override;
    ::grpc::ClientAsyncReaderWriter< ::grpc::reflection::v1alpha::ServerReflectionRequest, ::grpc::reflection::v1alpha::ServerReflectionResponse>* PrepareAsyncServerReflectionInfoRaw(::grpc::ClientContext* context, ::grpc::CompletionQueue* cq) override;
    const ::grpc::internal::RpcMethod rpcmethod_ServerReflectionInfo_;
  };
  static std::unique_ptr<Stub> NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options = ::grpc::StubOptions());

  class Service : public ::grpc::Service {
   public:
    Service();
    virtual ~Service();
    // The reflection service is structured as a bidirectional stream, ensuring
    // all related requests go to a single server.
    virtual ::grpc::Status ServerReflectionInfo(::grpc::ServerContext* context, ::grpc::ServerReaderWriter< ::grpc::reflection::v1alpha::ServerReflectionResponse, ::grpc::reflection::v1alpha::ServerReflectionRequest>* stream);
  };
  template <class BaseClass>
  class WithAsyncMethod_ServerReflectionInfo : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithAsyncMethod_ServerReflectionInfo() {
      ::grpc::Service::MarkMethodAsync(0);
    }
    ~WithAsyncMethod_ServerReflectionInfo() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status ServerReflectionInfo(::grpc::ServerContext* /*context*/, ::grpc::ServerReaderWriter< ::grpc::reflection::v1alpha::ServerReflectionResponse, ::grpc::reflection::v1alpha::ServerReflectionRequest>* /*stream*/)  override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestServerReflectionInfo(::grpc::ServerContext* context, ::grpc::ServerAsyncReaderWriter< ::grpc::reflection::v1alpha::ServerReflectionResponse, ::grpc::reflection::v1alpha::ServerReflectionRequest>* stream, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncBidiStreaming(0, context, stream, new_call_cq, notification_cq, tag);
    }
  };
  typedef WithAsyncMethod_ServerReflectionInfo<Service > AsyncService;
  template <class BaseClass>
  class WithCallbackMethod_ServerReflectionInfo : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithCallbackMethod_ServerReflectionInfo() {
      ::grpc::Service::MarkMethodCallback(0,
          new ::grpc::internal::CallbackBidiHandler< ::grpc::reflection::v1alpha::ServerReflectionRequest, ::grpc::reflection::v1alpha::ServerReflectionResponse>(
            [this](
                   ::grpc::CallbackServerContext* context) { return this->ServerReflectionInfo(context); }));
    }
    ~WithCallbackMethod_ServerReflectionInfo() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status ServerReflectionInfo(::grpc::ServerContext* /*context*/, ::grpc::ServerReaderWriter< ::grpc::reflection::v1alpha::ServerReflectionResponse, ::grpc::reflection::v1alpha::ServerReflectionRequest>* /*stream*/)  override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    virtual ::grpc::ServerBidiReactor< ::grpc::reflection::v1alpha::ServerReflectionRequest, ::grpc::reflection::v1alpha::ServerReflectionResponse>* ServerReflectionInfo(
      ::grpc::CallbackServerContext* /*context*/)
      { return nullptr; }
  };
  typedef WithCallbackMethod_ServerReflectionInfo<Service > CallbackService;
  typedef CallbackService ExperimentalCallbackService;
  template <class BaseClass>
  class WithGenericMethod_ServerReflectionInfo : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithGenericMethod_ServerReflectionInfo() {
      ::grpc::Service::MarkMethodGeneric(0);
    }
    ~WithGenericMethod_ServerReflectionInfo() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status ServerReflectionInfo(::grpc::ServerContext* /*context*/, ::grpc::ServerReaderWriter< ::grpc::reflection::v1alpha::ServerReflectionResponse, ::grpc::reflection::v1alpha::ServerReflectionRequest>* /*stream*/)  override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
  };
  template <class BaseClass>
  class WithRawMethod_ServerReflectionInfo : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithRawMethod_ServerReflectionInfo() {
      ::grpc::Service::MarkMethodRaw(0);
    }
    ~WithRawMethod_ServerReflectionInfo() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status ServerReflectionInfo(::grpc::ServerContext* /*context*/, ::grpc::ServerReaderWriter< ::grpc::reflection::v1alpha::ServerReflectionResponse, ::grpc::reflection::v1alpha::ServerReflectionRequest>* /*stream*/)  override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestServerReflectionInfo(::grpc::ServerContext* context, ::grpc::ServerAsyncReaderWriter< ::grpc::ByteBuffer, ::grpc::ByteBuffer>* stream, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncBidiStreaming(0, context, stream, new_call_cq, notification_cq, tag);
    }
  };
  template <class BaseClass>
  class WithRawCallbackMethod_ServerReflectionInfo : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithRawCallbackMethod_ServerReflectionInfo() {
      ::grpc::Service::MarkMethodRawCallback(0,
          new ::grpc::internal::CallbackBidiHandler< ::grpc::ByteBuffer, ::grpc::ByteBuffer>(
            [this](
                   ::grpc::CallbackServerContext* context) { return this->ServerReflectionInfo(context); }));
    }
    ~WithRawCallbackMethod_ServerReflectionInfo() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status ServerReflectionInfo(::grpc::ServerContext* /*context*/, ::grpc::ServerReaderWriter< ::grpc::reflection::v1alpha::ServerReflectionResponse, ::grpc::reflection::v1alpha::ServerReflectionRequest>* /*stream*/)  override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    virtual ::grpc::ServerBidiReactor< ::grpc::ByteBuffer, ::grpc::ByteBuffer>* ServerReflectionInfo(
      ::grpc::CallbackServerContext* /*context*/)
      { return nullptr; }
  };
  typedef Service StreamedUnaryService;
  typedef Service SplitStreamedService;
  typedef Service StreamedService;
};

}  // namespace v1alpha
}  // namespace reflection
}  // namespace grpc


#endif  // GRPC_src_2fproto_2fgrpc_2freflection_2fv1alpha_2freflection_2eproto__INCLUDED
