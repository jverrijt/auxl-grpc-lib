#Auxl-gRCP-lib

The primary purpose of this project is to provide gRPC services for generic API clients (i.e. Auxl). It exposes an easy-to-use C/C++ interface with the following functionality: 

- Retrieve descriptors through server reflection and/or provided proto files
- Generate JSON messages from gRPC types
- Send unary, server-, client-, and bidirectional streaming calls

Example: Retrieving the descriptors from a given service through server reflection and setting up and performing a unary call.


    GRPCConnectionOptions options = init_connection_options();
    auto connection = Connection::create_connection("localhost:5000", options);
        
    // Create descriptors from server reflection
    Descriptor descriptor({}, connection.get());
        
    // Create a message
    auto message = descriptor.create_message("greet.HelloRequest");
        
    // The method that we will call
    const auto method_descr = descriptor.get_method_descriptor("greet.Greeter.SayHello");
        
    if (method_descr != nullptr && message != nullptr)
    {
        Session session(connection.get());
        
        // A session delegates allows for capturing and formatting output.
        // It also captures errors
        DescriptorSessionDelegate delegate(&descriptor, "HelloResponse");
        session.delegate = &delegate;
        
        session.start(*method_descr);
        session.send_message(*message);
        session.close();
    }

##Command line tool

Get the descriptors for a given service that supports server reflection

    ./auxl_grpc_cli describe --endpoint url-to-grpc:port > descriptor.json

Create a message of type `greet.HelloRequest` and write it out to `message.json`

    ./auxl_grpc_cli message --type_name greet.HelloRequest --descriptors descriptor.json > message.json

Edit the message and call the service method `greet.Greeter.SayHello`

    ./auxl_grpc_cli call --endpoint url-to-grpc:port --descriptors descriptor.json  --method greet.Greeter.SayHello --message message.json

For bidirectional- and client stream calls, add multiple `--message` options. A menu will be presented to allow selecting the message to be send to the stream.


###Connection options

Pass along connection options to the describe and call commands by setting the `—connection_options` to a json file with the following format.

    {
        "timeout": 100,
        "use_ssl": true,
        "ssl_client_cert": "",
        "ssl_client_key": "",
        "ssl_root_certs_path": “path to file that contains the certificate authorities“
    }

Omit or leave blank any value that’s not pertinent. To use SSL, at a minimum, `use_ssl` needs to be set to true and the `ssl_root_certs_path` needs to be set to a valid cacert bundle ([e.g. Mozilla's root certificates](https://github.com/gisle/mozilla-ca/blob/master/lib/Mozilla/CA/cacert.pem))


## Building

Compile from source or use a package manager (e.g. brew, apt) to install the following prerequisites:

- gRPC library [https://github.com/grpc/grpc/blob/master/BUILDING.md](https://github.com/grpc/grpc/blob/master/BUILDING.md)
- Protocol Buffer Compiler [https://grpc.io/docs/protoc-installation/](https://grpc.io/docs/protoc-installation/)

Then, in the root folder of the project:

    ./generate-reflection-support.sh
    ./autogen.sh
    ./configure && make


### Unit tests

The unit tests require gtest. They are included in GRPC's source distribution. To install, go into `modules/` and run: 

    git submodule update --init


