# TcpServer with LibUV Example

A sample networking implementation, in C++, using libuv async I/O library.

## Prerequisites
* gcc
* CMake (min 3.4)
* libuv

## Usage

    git clone https://github.com/catenocrypt/tcpserver-libuv-sample.git
    cd tcpserver-libuv-sample
    cmake .
    make
    ./tcp-libuv-server

From another console:

    ./tcp-libuv-client

## Notes

* Connections are TCP connections
* The server uses one UV loop, for the listening socket and for all client sockets
* In the server UV loop is in a background thread, can be stopped from other thread (using UV async handle)
* The client uses one UV loop for the client sockets, in the main thread
* Messages are encoded simple text-based, variable-length, using terminators and separators.
