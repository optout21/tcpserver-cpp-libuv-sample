#pragma once

#include "uv_socket.hpp"

#include <memory>
#include <string>
#include <thread>

namespace sample
{
    class BaseApp; // forward

    class NetHandler: public IUvSocket
    {
    public:
        NetHandler(BaseApp* app_in);
        static uv_loop_t* getUvLoop();
        static void deleteUvLoop();
        static int runLoop();
        int start();
        // return actual listen port
        int startWithListen(int port_in, int tryNextPorts_in);
        int stop();
        void onNewConnection(uv_stream_t* server, int status);
        /// Obtain the remote endpoint (host:port) of a connected socket
        static std::string getRemoteAddress(const uv_tcp_t* socket_in);

    private:
        int startUvLoop();
        int stopUvLoop();
        int doBindAndListen(int port_in);
        // return actual listen port
        int doListen(int port_in, int tryNextPorts_in);
        int doBgThread();
        static void on_new_connection(uv_stream_t* server, int status);
        static void on_close(uv_handle_t* handle);
        static void on_walk(uv_handle_t* handle, void* arg);

    private:
        static uv_loop_t* myUvLoop;
        BaseApp* myApp;
        uv_async_t* myUvAsync;
        std::thread myBgThread;
        bool myBgThreadStop;
    };
}
