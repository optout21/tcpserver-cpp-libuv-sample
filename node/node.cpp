#include "node.hpp"
#include "peer_conn.hpp"
#include "endpoint.hpp"
#include "../lib/net_handler.hpp"
#include "../lib/net_client.hpp"

#include <cassert>
#include <iostream>

using namespace sample;
using namespace std;


NodeApp::PeerCandidateInfo::PeerCandidateInfo(std::string host_in, int port_in, bool sticky_in) :
myHost(host_in),
myPort(port_in),
myStickyFlag(sticky_in),
myConnTryCount(0),
myConnectedCount(0)
{
}


void NodeApp::PeerInfo::setOutClient(std::shared_ptr<PeerClientOut>& outClient_in)
{
    myOutClient = outClient_in;
}

void NodeApp::PeerInfo::setInClient(std::shared_ptr<NetClientIn>& inClient_in)
{
    myInClient = inClient_in;
}

void NodeApp::PeerInfo::resetOutClient()
{
    myOutClient = nullptr;
}

void NodeApp::PeerInfo::resetInClient()
{
    myOutClient = nullptr;
}


NodeApp::NodeApp() :
ServerApp()
{
    myNetHandler = new NetHandler(this);
}

void NodeApp::start(AppParams const & appParams_in)
{
    // add constant peer candidates, for localhost
    int n = 2;
    for (int i = 0; i < n; ++i)
    {
        addOutPeerCandidate("localhost", 5000 + i, true);
    }
    // add extra peer candidates
    if (appParams_in.extraPeers.size() > 0)
    {
        for(int i = 0; i < appParams_in.extraPeers.size(); ++i)
        {
            if (appParams_in.extraPeers[i].length() > 0)
            {
                Endpoint extraPeerEp(appParams_in.extraPeers[i]);
                addOutPeerCandidate(extraPeerEp.getHost(), extraPeerEp.getPort(), true);
            }
        }
    }

    int actualPort = myNetHandler->startWithListen(appParams_in.listenPort, appParams_in.listenPortRange);
    if (actualPort <= 0) return;
    myName = ":" + to_string(actualPort);
}

void NodeApp::listenStarted(int port)
{
    cout << "App: Listening on port " << port << endl;
    // try to connect to clients
    tryOutConnections();
}


void NodeApp::addOutPeerCandidate(std::string host_in, int port_in, bool sticky_in)
{
    PeerCandidateInfo pc = PeerCandidateInfo(host_in, port_in, sticky_in);
    string key = host_in + ":" + to_string(port_in);
    if (myPeerCands.find(key) != myPeerCands.end())
    {
        // already present
        return;
    }
    myPeerCands[key] = pc;
    cout << "App: Added peer candidate " << key << " " << myPeerCands.size() << endl;
    //debugPrintPeerCands();
}

void NodeApp::debugPrintPeerCands()
{
    cout << "PeerCands: " << myPeerCands.size() << "  ";
    for (auto i = myPeerCands.begin(); i != myPeerCands.end(); ++i)
    {
        cout << "[" << i->first << " " << i->second.myStickyFlag << " " << i->second.myConnTryCount << ":" << i->second.myConnectedCount << "] ";
    }
    cout << endl;
}

void NodeApp::debugPrintPeers()
{
    cout << "Peers: " << myPeers.size() << "  ";
    for (auto i = myPeers.begin(); i != myPeers.end(); ++i)
    {
        cout << "[" << i->first << " " << (i->second.myOutClient == nullptr ? "n" : (i->second.myOutClient->isConnected() ? "Y" : "N")) << "] ";
    }
    cout << endl;
}

void NodeApp::tryOutConnections()
{
    //cout << "NodeApp::tryOutConnections" << endl;
    // try to connect to clients
    for (auto i = myPeerCands.begin(); i != myPeerCands.end(); ++i)
    {
        //cout << i->second.myOutFlag << " " << i->second.myStickyFlag << " " << i->second.myConnTryCount << " " << (i->second.myOutClient == nullptr) << " " << i->second.myEndpoint.getEndpoint() << endl;
        if (i->second.myConnTryCount == 0 || i->second.myStickyFlag)
        {
            if (!isPeerOutConnected(i->first))
            {
                // try outgoing connection
                ++i->second.myConnTryCount;
                int res = tryOutConnection(i->second.myHost, i->second.myPort);
                if (!res)
                {
                    // success
                    ++i->second.myConnectedCount;
                }
            }
        }
    }
}

bool NodeApp::isPeerOutConnected(string peerAddr_in)
{
    if (myPeers.find(peerAddr_in) == myPeers.end())
    {
        // no such peer
        return false;
    }
    if (myPeers[peerAddr_in].myOutClient == nullptr)
    {
        // no out client
        return false;
    }
    return myPeers[peerAddr_in].myOutClient->isConnected();
}

int NodeApp::tryOutConnection(std::string host_in, int port_in)
{
    // exclude localhost connections to self
    if ((":" + to_string(port_in)) == myName)
    {
        if (host_in == "localhost" || host_in == "127.0.0.1" || host_in == "::1" || host_in == "[::1]")
        {
            cerr << "Ignoring peer candidate to self (" << host_in << ":" << port_in << ")" << endl;
            return 0;
        }
    }
    // try outgoing connection
    Endpoint ep = Endpoint(host_in, port_in);
    string key = ep.getEndpoint();
    //cout << "Trying outgoing conn to " << key << endl;
    auto peerout = make_shared<PeerClientOut>(this, host_in, port_in);
    if (myPeers.find(key) == myPeers.end())
    {
        myPeers[key] = PeerInfo();
    }
    myPeers[key].setOutClient(peerout);
    int res = peerout->connect();
    if (res)
    {
        cerr << "Error from peer connect, " << res << endl;
        return res;
    }
    //debugPrintPeers();
    return 0;
}

void NodeApp::stop()
{
    myNetHandler->stop();
}

void NodeApp::inConnectionReceived(std::shared_ptr<NetClientBase>& client_in)
{
    assert(client_in != nullptr);
    string cliaddr = client_in->getPeerAddr();
    cout << "App: New incoming connection: " << cliaddr << endl;
    if (myPeers.find(cliaddr) == myPeers.end())
    {
        myPeers[cliaddr] = PeerInfo();
    }
    auto cliIn = dynamic_pointer_cast<NetClientIn>(client_in);
    if (cliIn == nullptr)
    {
        cerr << "Error null incoming client" << endl;
    }
    myPeers[cliaddr].setInClient(cliIn);
    //debugPrintPeers();
}

void NodeApp::connectionClosed(NetClientBase* client_in)
{
    assert(client_in != nullptr);
    string cliaddr = client_in->getPeerAddr();
    cout << "App: Connection done: " << cliaddr << endl;
    for(auto i = myPeers.begin(); i != myPeers.end(); ++i)
    {
        if (i->second.myInClient.get() == client_in)
        {
            i->second.resetInClient();
        }
        if (i->second.myOutClient.get() == client_in)
        {
            i->second.resetOutClient();
        }
    }
    // remove disconnected clients
    bool changed = true;
    while (changed)
    {
        changed = false;
        for(auto i = myPeers.begin(); i != myPeers.end(); ++i)
        {
            if (i->second.myOutClient == nullptr && i->second.myInClient == nullptr)
            {
                cout << "Removing disconnected client " << myPeers.size() << " " << i->first << endl;
                myPeers.erase(i->first);
                //cout << "Removed disconnected client " << myPeers.size() << " " << i->first << endl;
                changed = true;
                break; // for
            }
        }
    } 
}

void NodeApp::messageReceived(NetClientBase & client_in, BaseMessage const & msg_in)
{
    if (msg_in.getType() != MessageType::OtherPeer)
    {
        cout << "App: Received: from " << client_in.getPeerAddr() << " '" << msg_in.toString() << "'" << endl;
    }
    switch (msg_in.getType())
    {
        case MessageType::Handshake:
            {
                HandshakeMessage const & hsMsg = dynamic_cast<HandshakeMessage const &>(msg_in);
                //cout << "Handshake message received, '" << hsMsg.getMyAddr() << "'" << endl;
                if (hsMsg.getMyVersion() != "V01")
                {
                    cerr << "Wrong version ''" << hsMsg.getMyVersion() << "'" << endl;
                    client_in.close();
                    return;
                }
                HandshakeResponseMessage resp("V01", myName, client_in.getPeerAddr());
                client_in.sendMessage(resp);
                // try to connect ougoing too, host taken from actual remote peer, port reported by peer
                string reportedPeerName = hsMsg.getMyAddr();
                if (reportedPeerName.substr(0, 1) == ":")
                {
                    string port = reportedPeerName.substr(1);
                    string host = Endpoint(client_in.getPeerAddr()).getHost();
                    addOutPeerCandidate(host, stoi(port), false);
                }
                tryOutConnections();
                //sendOtherPeers(client_in);
            }
            break;

        case MessageType::Ping:
            {
                PingMessage const & pingMsg = dynamic_cast<PingMessage const &>(msg_in);
                //cout << "Ping message received, '" << pingMsg.getText() << "'" << endl;
                PingResponseMessage resp("Resp_from_" + myName + "_to_" + pingMsg.getText());
                client_in.sendMessage(resp);
            }
            break;

        case MessageType::OtherPeer:
            {
                OtherPeerMessage const & peerMsg = dynamic_cast<OtherPeerMessage const &>(msg_in);
                //cout << "OtherPeer message received, " << peerMsg.getHost() << ":" << peerMsg.getPort() << " " << peerMsg.toString() << endl;
                addOutPeerCandidate(peerMsg.getHost(), peerMsg.getPort(), false);
                tryOutConnections();
            }
            break;

        case MessageType::HandshakeResponse:
        case MessageType::PingResponse:
            // OK, noop
            break;

        default:
            assert(false);
    }
}

void NodeApp::sendOtherPeers(NetClientBase & client_in)
{
    // send current outgoing connection addresses
    auto peers = getConnectedPeers();
    //cout << "NodeApp::sendOtherPeers " << peers.size() << " " << client_in.getPeerAddr() << endl;
    for(auto i = peers.begin(); i != peers.end(); ++i)
    {
        if (!client_in.isConnected())
        {
            return;
        }
        string ep = i->getEndpoint();
        //cout << ep << " " << client_in.getPeerAddr() << endl;
        if (ep != client_in.getPeerAddr())
        {
            //cout << "sendOtherPeers " << client_in.getPeerAddr() << " " << i->getEndpoint() << endl;
            client_in.sendMessage(OtherPeerMessage(i->getHost(), i->getPort()));
        }
    }
}

vector<Endpoint> NodeApp::getConnectedPeers() const
{
    vector<Endpoint> peers;
    for(auto i = myPeers.begin(); i != myPeers.end(); ++i)
    {
        // TODO if  ((i->second.myOutClient != nullptr && i->second.myOutClient->isConnected()) ||
        //    ((i->second.myInClient != nullptr && i->second.myInClient->isConnected())))
        if ((i->second.myOutClient != nullptr && i->second.myOutClient->isConnected()))
        {
            peers.push_back(i->first);
        }
    }
    return peers;
}
