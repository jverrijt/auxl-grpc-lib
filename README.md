auxl_grpc_cli

auxl_grpc_cli is an extension on the gRPC libs to help in building generic API clients (ie Auxl). It provides an easy-to-use interface, either through reflection or provided proto files, to retrieve the descriptors of a given service. It can generate messages and call services with support for unary, server streaming, client streaming, and bidirectional streaming. Additionally, it contains a command line tool do perform common gRPC actions.


## Example 

``

``






Command line tool


Example





Connection options

Pass along connection options to the describe and call commands by setting the —connection_options opby specifying them in a json file with the following format.

{
"timeout": 100,
    "use_ssl": true,
    "ssl_client_cert": "",
    "ssl_client_key": "",
    "ssl_root_certs_path": “path to file that contains the certificate authorities“
}

Omit or leave blank any value that’s not pertinent. To use SSL, at a minimum, 



## Building


