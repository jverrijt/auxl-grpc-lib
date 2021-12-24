CXX = clang++
CXXFLAGS = -g -O0 -Wpedantic -Wno-unused-command-line-argument -std=c++11 -I./modules/grpc/cmake/build/gens/ -I./modules/grpc/

# absl dependencies
LIB_ABSL_LIB_PATH = /usr/local/lib
LIB_ABSL_LIBS = $(wildcard $(LIB_ABSL_LIB_PATH)/libabsl_*.a)

LDFLAGS_GRPC = -lgrpc -lgrpc++ -lgrpc++_reflection -lgrpcpp_channelz -lprotobuf -lupb -lgpr -lz -lre2 -lcares -laddress_sorting -lpthread \
	/usr/local/lib/libssl.a /usr/local/lib/libcrypto.a
LDFLAGS_CLI = -L./lib -L/usr/local/lib -framework CoreFoundation

ODIR = obj

OBJS_CLI = src/auxl_cli.o
OBJS_LIB = src/parser.o \
		src/util.o \
		src/connection.o \
		modules/grpc/test/cpp/util/proto_reflection_descriptor_database.o \
		modules/grpc/cmake/build/gens/src/proto/grpc/reflection/v1alpha/reflection.grpc.pb.o

.PHONY: clean

$(ODIR)/%.o: $(CLI_SRCS)
	$(CXX) -c -o $(ODIR)/$(notdir $@) $< $(CXXFLAGS) $(LDFLAGS_GRPC)

bin/auxl_cli: $(OBJS_LIB) $(OBJS_CLI)
	@echo ":: Executing auxl cli target ::"
	$(CXX) $(CXXFLAGS) $(LDFLAGS_CLI) $(LDFLAGS_GRPC) /usr/local/lib/libssl.a /usr/local/lib/libcrypto.a $(LIB_ABSL_LIBS) -o $@ $^

libauxl_grpc.a : $(OBJS_LIB)
	@echo ":: AUXL static lib target ::"
	mkdir -p lib
	ar rcs lib/$@ $^

clean:
	rm -f bin/*
	rm -f src/*.o
	rm -f lib/*.a
	rm -f *.o
