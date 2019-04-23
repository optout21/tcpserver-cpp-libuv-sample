#pragma once

#include "../lib/app.hpp"

namespace sample
{
    class NetHandler; // forward
    class NetClientBase; // forward

    class NodeApp: public BaseApp
    {
    public:
        NodeApp();
        /// Start listening and the UV processing loop
        void start();
        /// Stop the background thread loop, stop listening
        void stop();
        /// Called when server is listening on a port already
        virtual void listenStarted(int port);
        /// Called when an incoming message is reveiced
        void messageReceived(NetClientBase & client_in, BaseMessage const & msg_in);
    protected:
        NetHandler* myNetHandler;
        std::string myName;
    };
}
