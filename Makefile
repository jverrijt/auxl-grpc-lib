CXX = clang++

# Fix the include path
CXXFLAGS = -g -O0 -Wpedantic -Wno-unused-command-line-argument -std=c++11 \
 -I./include \
 -I./modules/grpc/third_party/googletest/googletest/include \
 -I./modules/grpc/third_party/googletest/googletest \
 -I./src \
 -I./src/vendor

# absl dependencies
LIB_ABSL_LIB_PATH = /usr/local/lib
LIB_ABSL_LIBS = $(wildcard $(LIB_ABSL_LIB_PATH)/libabsl_*.a)

LDFLAGS_GRPC = -lgrpc -lgrpc++ -lgrpc++_reflection -lgrpcpp_channelz -lprotobuf -lupb -lgpr -lz -lre2 -lcares -laddress_sorting -lpthread \
	/usr/local/lib/libssl.a /usr/local/lib/libcrypto.a

LDFLAGS_CLI = -L./lib -L/usr/local/lib -framework CoreFoundation

ODIR = obj

OBJS_TST = test/auxl_main_test.o \
	modules/grpc/third_party/googletest/googletest/src/gtest-all.o \
	modules/grpc/third_party/googletest/googletest/src/gtest_main.o

OBJS_CLI = src/cli/auxl_grpc_cli.o \
		src/cli/cli_util.cpp
OBJS_LIB = src/parser.o \
		src/util.o \
		src/connection.o \
		src/descriptor.o \
		src/session.o \
		src/options.o \
		src/error_collector.o \
		src/vendor/proto_reflection_descriptor_database.o \
		src/vendor/reflection.grpc.pb.o \
		src/vendor/cli_call.o

.PHONY: clean

$(ODIR)/%.o: $(CLI_SRCS)
	$(CXX) -c -o $(ODIR)/$(notdir $@) $< $(CXXFLAGS) $(LDFLAGS_GRPC)

bin/auxl_grpc_cli: $(OBJS_LIB) $(OBJS_CLI)
	@echo ":: Executing CLI target ::"
	$(CXX) $(CXXFLAGS) $(LDFLAGS_CLI) $(LDFLAGS_GRPC) /usr/local/lib/libssl.a /usr/local/lib/libcrypto.a $(LIB_ABSL_LIBS) -o $@ $^

libauxl_grpc.a : $(OBJS_LIB)
	@echo ":: AUXL static lib target ::"
	mkdir -p lib
	ar rcs lib/$@ $^


test: bin/auxl_grpc_test

# build and run the test
bin/auxl_grpc_test: $(OBJS_LIB) $(OBJS_TST)
	@echo ":: Executing TEST target ::"
	$(CXX) $(CXXFLAGS) $(LDFLAGS_CLI) $(LDFLAGS_GRPC) /usr/local/lib/libssl.a /usr/local/lib/libcrypto.a $(LIB_ABSL_LIBS) -o $@ $^
	@bin/auxl_grpc_test


# convenience to create Xcode linker flags
list_ld_flags:
	@echo "$(LDFLAGS_GRPC)"
	@echo "$(subst lib,-l,$(basename $(notdir $(LIB_ABSL_LIBS))))"
	@echo "$(LDFLAGS_CLI)"


clean:
	rm -f bin/*
	rm -f src/*.o
	rm -f lib/*.a
	rm -f *.o
