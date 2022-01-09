AUTOMAKE_OPTIONS = subdir-objects
AM_CXXFLAGS = -std=c++11 -I$(srcdir)/src -I$(srcdir)/include -I$(srcdir)/src/vendor
AM_LDFLAGS = 

if OSX
AM_LDFLAGS += -framework CoreFoundation
endif


noinst_LIBRARIES = libauxl_grpc.a
libauxl_grpc_a_SOURCES = src/parser.cpp \
		src/util.cpp \
		src/connection.cpp \
		src/descriptor.cpp \
		src/session.cpp \
		src/options.c \
		src/error_collector.c \
		src/vendor/proto_reflection_descriptor_database.cc \
		src/vendor/reflection.grpc.pb.cc \
		src/vendor/cli_call.cc \
		src/vendor/reflection.pb.cc

lib_auxl_grpc_a_CFLAGS = 

bin_PROGRAMS = auxl_grpc_cli
auxl_grpc_cli_SOURCES = src/cli/auxl_grpc_cli.cpp \
		src/cli/cli_util.cpp

auxl_grpc_cli_LDADD = libauxl_grpc.a

