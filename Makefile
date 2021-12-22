CXX = clang++ 
CXXFLAGS = -g -Wall -std=c++11

CLI_SRC = \
		  src/auxl_cli.o

LIB_SRC = \
		  src/parser.o \
		  src/tool.o \
		  src/util.o \
		  src/connection.o


LIB_ABSL_LIB_PATH = /usr/local/lib
LIB_ABSL_LIBS = $(wildcard $(LIB_ABSL_LIB_PATH)/libabsl_*.a)
# LIB_ABSL_A = $(addprefix -l , $(LIB_ABSL_LIBS))

# $(filter %.a,$(LIB_ABSL_LIB_PATH))

LDFLAGS_GRPC = -lgrpc -lgrpc++ -lgrpcpp_channelz -lprotobuf -lupb -lgpr -lz -lre2 -lcares -laddress_sorting
LDFLAGS_CLI = -L./lib -L/usr/local/lib -lauxl_grpc -framework CoreFoundation

.PHONY: clean

cli: libauxl_grpc auxl_cli

auxl_cli: $(CLI_SRC) 
	@echo "==> Building CLI"
	mkdir -p bin
	$(CXX) $(CXXFLAGS) $(LDFLAGS_CLI) $(LDFLAGS_GRPC) /usr/local/lib/libssl.a /usr/local/lib/libcrypto.a $(LIB_ABSL_LIBS) -o bin/$@ $<

libauxl_grpc: $(LIB_SRC) 
	@echo "==> BUILDING LIB"
	mkdir -p bin
	mkdir -p lib
	$(CXX) -c $^ $(CXXFLAGS) $(LDFLAGS_GRPC)

	ar rcs lib/$@.a $^

clean:
	rm -f bin/*
	rm -f src/*.o
	rm -f lib/*.a
