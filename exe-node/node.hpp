#pragma once

#include "../lib/app.hpp"

#include <map>
#include <memory>

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
        /// Called when a new incoming connection is received
        void inConnectionReceived(std::shared_ptr<NetClientBase>& client_in);
        /// Called when an incoming connection has finished
        void connectionClosed(std::shared_ptr<NetClientBase>& client_in);
        /// Called when an incoming message is received
        void messageReceived(NetClientBase & client_in, BaseMessage const & msg_in);

    protected:
        NetHandler* myNetHandler;
        std::string myName;
        std::map<std::string, std::shared_ptr<NetClientBase>> myClients;
    };
}
