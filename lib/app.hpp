#pragma once

#include <string>

namespace sample
{
    class BaseMessage; // forward
    class NetClientBase; // forward
    class NetHandler; // forward

    /*
     * Application main class, base class.
     */
    class BaseApp
    {
    public:
        BaseApp() = default;
        virtual void start() = 0;
        /// Called when server is listening on a port already
        virtual void listenStarted(int port);
        virtual void messageReceived(NetClientBase & client_in, BaseMessage const & msg_in) = 0;
    };

    class ServerApp: public BaseApp
    {
    public:
        ServerApp();
        /// Start listening and the UV processing loop
        void start();
        /// Stop the background thread loop, stop listening
        void stop();
        /// Called when an incoming message is reveiced
        void messageReceived(NetClientBase & client_in, BaseMessage const & msg_in);
    protected:
        NetHandler* myNetHandler;
        std::string myName;
    };

    class ClientApp: public BaseApp
    {
    public:
        ClientApp();
        /// Start the clients, connect, process events
        void start();
        void messageReceived(NetClientBase & client_in, BaseMessage const & msg_in);
    };
}