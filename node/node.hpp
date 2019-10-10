#pragma once

#include "endpoint.hpp"
#include "../lib/app.hpp"

#include <map>
#include <memory>
#include <vector>

namespace sample
{
    class NetHandler; // forward
    class NetClientIn; // forward
    class PeerClientOut; // forward

    class NodeApp: public ServerApp
    {
    public:
        NodeApp();
        /// Start listening and the UV processing loop
        virtual void start(AppParams const & appParams_in);
        /// Stop the background thread loop, stop listening
        void stop();
        void sendOtherPeers(NetClientBase & client_in);

    protected:
        /// Called when server is listening on a port already
        virtual void listenStarted(int port);
        void addOutPeerCandidate(std::string host_in, int port_in, bool sticky_in);
        void tryOutConnections();
        bool isPeerOutConnected(std::string peerAddr_in);
        int tryOutConnection(std::string host_in, int port_in);
        void debugPrintPeerCands();
        void debugPrintPeers();
        /// Called when a new incoming connection is received
        void inConnectionReceived(std::shared_ptr<NetClientBase>& client_in);
        /// Called when an incoming connection has finished
        void connectionClosed(NetClientBase* client_in);
        /// Called when an incoming message is received
        void messageReceived(NetClientBase & client_in, BaseMessage const & msg_in);
        // Send all known active out peer connections to this peer
        virtual std::string getName() { return myName; }
        std::vector<Endpoint> getConnectedPeers() const;

    protected:
        class PeerCandidateInfo
        {
        public:
            PeerCandidateInfo() = default;
            PeerCandidateInfo(std::string host_in, int port_in, bool sticky_in);

        public:
            std::string myHost;
            int myPort;
            int myConnTryCount;
            int myConnectedCount;
            bool myStickyFlag;
        };

        class PeerInfo
        {
        public:
            PeerInfo() = default;
            void setOutClient(std::shared_ptr<PeerClientOut>& outClient_in);
            void setInClient(std::shared_ptr<NetClientIn>& inClient_in);
            void resetOutClient();
            void resetInClient();

        public:
            // connection towards this peer; may be null
            std::shared_ptr<PeerClientOut> myOutClient;
            // connection from this peer; may be null
            std::shared_ptr<NetClientIn> myInClient;
        };

    private:
        NetHandler* myNetHandler;
        std::string myName;
        // peer candidates (outgoing connections to try)
        std::map<std::string, PeerCandidateInfo> myPeerCands;
        // current peer connections
        std::map<std::string, PeerInfo> myPeers;
    };
}
