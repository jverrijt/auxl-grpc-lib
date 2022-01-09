#!/bin/sh

protoc gen/reflection.proto --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` --grpc_out=./gen
protoc gen/reflection.proto --cpp_out=./gen
