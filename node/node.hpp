#pragma once

#include "endpoint.hpp"
#include "../lib/app.hpp"

#include <map>
#include <memory>
#include <vector>
#include <list>

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
        void addOutPeerCandidate(std::string host_in, int port_in, int toTry_in);
        void tryOutConnections();
        bool isPeerConnected(std::string peerAddr_in, bool outDir_in);
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
            PeerCandidateInfo(std::string host_in, int port_in, int toTry_in);

        public:
            std::string myHost;
            int myPort;
            int myToTry; // how many times to try
            int myConnTryCount; // no of connection trials
            int myConnectedCount; // no of successful connections
        };

        class PeerInfo
        {
        public:
            PeerInfo() = default;
            void setClient(std::shared_ptr<NetClientBase>& client_in);
            void resetClient();

        public:
            // current connections
            std::shared_ptr<NetClientBase> myClient;
            bool myOutFlag;
        };

    private:
        NetHandler* myNetHandler;
        std::string myName;
        // peer candidates (outgoing connections to try)
        std::map<std::string, PeerCandidateInfo> myPeerCands;
        // current peer connections
        std::list<PeerInfo> myPeers;
    };
}
