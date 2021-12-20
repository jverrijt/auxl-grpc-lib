CXX = clang++ 
CXXFLAGS = -g -Wall

CLI_SRC = \
		  src/auxl_cli.o

LIB_SRC = \
		  src/auxl_grpc_parser.o \
		  src/auxl_grpc_tool.o \

cli: libauxl_grpc auxl_cli

auxl_cli: $(CLI_SRC) 
	mkdir -p bin
	$(CXX) $(CXXFLAGS) -L./lib -lauxl_grpc -o bin/$@ $<

libauxl_grpc: $(LIB_SRC) 
	mkdir -p bin
	mkdir -p lib
	$(CXX) -c $^ $(CXXFLAGS)
	ar rc lib/$@.a $^

clean:
	rm -f bin/*
	rm -f src/*.o
	rm -f lib/*.a