#pragma once

#include "../lib/app.hpp"

#include <map>
#include <memory>

namespace sample
{
    class NetHandler; // forward
    class NetClientIn; // forward
    class PeerClientOut; // forward

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
        void addStickyPeer(std::string host_in, int port_in);
        void tryOutConnections();
        /// Called when a new incoming connection is received
        void inConnectionReceived(std::shared_ptr<NetClientBase>& client_in);
        /// Called when an incoming connection has finished
        void connectionClosed(NetClientBase* client_in);
        /// Called when an incoming message is received
        void messageReceived(NetClientBase & client_in, BaseMessage const & msg_in);

        class PeerInfo
        {
        public:
            PeerInfo() = default;
            PeerInfo(std::string endpoint_in);
            PeerInfo(std::string outHost_in, int outPort_in);
            void setOutClient(std::shared_ptr<PeerClientOut>& outClient_in);
            void setInClient(std::shared_ptr<NetClientIn>& inClient_in);
            void resetOutClient();
            void resetInClient();

        public:
            std::string myEndpoint;
            int myOutConnTryCount;
            // connection towards this peer; may be null
            std::shared_ptr<PeerClientOut> myOutClient;
            // connection from this peer; may be null
            std::shared_ptr<NetClientIn> myInClient;
            std::string myOutHost;
            int myOutPort;
            bool myStickyOutFlag;
        };

    protected:
        NetHandler* myNetHandler;
        std::string myName;
        std::map<std::string, PeerInfo> myPeers;
    };
}
