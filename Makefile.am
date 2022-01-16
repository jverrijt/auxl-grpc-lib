AUTOMAKE_OPTIONS = subdir-objects
AM_CXXFLAGS = -std=c++11 -I$(srcdir)/src -I$(srcdir)/include -I$(srcdir)/src/vendor -I$(srcdir)/gen -I$(srcdir)/src/extern

noinst_LIBRARIES = libauxl_grpc.a
libauxl_grpc_a_SOURCES = src/parser.cpp \
		src/util.cpp \
		src/connection.cpp \
		src/descriptor.cpp \
		src/session.cpp \
		src/extern/options.c \
		src/extern/error_collector.c \
		src/extern/tool.cpp \
		src/vendor/proto_reflection_descriptor_database.cc \
		gen/gen/reflection.grpc.pb.cc \
		gen/gen/reflection.pb.cc \
		src/vendor/cli_call.cc

lib_auxl_grpc_a_CFLAGS = 

bin_PROGRAMS = auxl_grpc_cli
auxl_grpc_cli_SOURCES = src/cli/auxl_grpc_cli.cpp \
		src/cli/cli_util.cpp

auxl_grpc_cli_LDADD = libauxl_grpc.a
