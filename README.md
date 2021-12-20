Main C/C++ based GRPC module for Auxl


Build GRPC but with openssl

``
cmake -G Xcode -T buildsystem=1 -DgRPC_BUILD_TESTS=ON -DgRPC_SSL_PROVIDER=OpenSSL -DCMAKE_CXX_FLAGS=-I\ /usr/local/include\ -L/Users/joostverrijt/Projects/var/temp/lib\ -lssl\ -lcrypto ../..
``
