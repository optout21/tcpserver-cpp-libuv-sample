#pragma once

#include <map>
#include <memory>
#include <string>

namespace sample
{
    class BaseMessage; // forward
    class NetClientBase; // forward
    class NetHandler; // forward

    /**
     * Params for the app.
     */
    struct AppParams
    {
    public:
        AppParams(std::string extraPeer_in, int listenPort_in, int listenPortRange_in)
        {
            extraPeer = extraPeer_in;
            listenPort = listenPort_in;
            listenPortRange = listenPortRange_in;
        }

        std::string extraPeer;
        int listenPort;
        int listenPortRange;

        void print();
    };

    /*
     * Application main class, base class.
     */
    class BaseApp
    {
    public:
        BaseApp() = default;
        virtual void start(AppParams const & appParams_in) = 0;
        /// Called when server is listening on a port already
        virtual void listenStarted(int port);
        /// Called when a new incoming connection is received
        virtual void inConnectionReceived(std::shared_ptr<NetClientBase>& client_in) = 0;
        /// Called when an incoming connection has finished
        virtual void connectionClosed(NetClientBase* client_in) = 0;
        /// Called when an incoming message is received
        virtual void messageReceived(NetClientBase & client_in, BaseMessage const & msg_in) = 0;
        virtual std::string getName() { return "_NONE_"; }
    };

    class ServerApp: public BaseApp
    {
    public:
        ServerApp();
        /// Start listening and the UV processing loop
        virtual void start(AppParams const & appParams_in);
        /// Stop the background thread loop, stop listening
        void stop();
        /// Called when a new incoming connection is received
        void inConnectionReceived(std::shared_ptr<NetClientBase>& client_in);
        /// Called when an incoming connection has finished
        void connectionClosed(NetClientBase* client_in);
        /// Called when an incoming message is received
        void messageReceived(NetClientBase & client_in, BaseMessage const & msg_in);
        virtual std::string getName() { return myName; }
        
    protected:
        NetHandler* myNetHandler;
        std::string myName;
        std::map<std::string, std::shared_ptr<NetClientBase>> myClients;
    };

    class ClientApp: public BaseApp
    {
    public:
        ClientApp();
        /// Start the clients, connect, process events
        virtual void start(AppParams const & appParams_in);
        void inConnectionReceived(std::shared_ptr<NetClientBase>& client_in) { }
        void connectionClosed(NetClientBase* client_in) { }
        void messageReceived(NetClientBase & client_in, BaseMessage const & msg_in);
    };
}