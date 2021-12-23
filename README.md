Main C/C++ based GRPC module for Auxl


Build GRPC but with openssl

``
cmake -DgRPC_SSL_PROVIDER=OpenSSL -DCMAKE_CXX_FLAGS=-I\ /usr/local/include\ -L/Users/joostverrijt/Projects/var/temp/lib\ -lssl\ -lcrypto ../..
``
