#include "node.hpp"
#include "peer_conn.hpp"
#include "endpoint.hpp"
#include "../lib/net_handler.hpp"
#include "../lib/net_client.hpp"

#include <cassert>
#include <iostream>

using namespace sample;
using namespace std;


NodeApp::PeerCandidateInfo::PeerCandidateInfo(std::string host_in, int port_in, int toTry_in) :
myHost(host_in),
myPort(port_in),
myToTry(toTry_in),
myConnTryCount(0),
myConnectedCount(0)
{
}


void NodeApp::PeerInfo::setClient(std::shared_ptr<NetClientBase>& client_in)
{
    myClient = client_in;
}



void NodeApp::PeerInfo::resetClient()
{
    myClient = nullptr;
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
        addOutPeerCandidate("localhost", 5000 + i, 1);
    }
    // add extra peer candidates
    if (appParams_in.extraPeers.size() > 0)
    {
        for(int i = 0; i < appParams_in.extraPeers.size(); ++i)
        {
            if (appParams_in.extraPeers[i].length() > 0)
            {
                Endpoint extraPeerEp(appParams_in.extraPeers[i]);
                addOutPeerCandidate(extraPeerEp.getHost(), extraPeerEp.getPort(), 1000000);
            }
        }
    }

    myNetHandler->startWithListen(appParams_in.listenPort, appParams_in.listenPortRange);
}

void NodeApp::listenStarted(int port)
{
    cout << "App: Listening on port " << port << endl;
    myName = ":" + to_string(port);
    // try to connect to clients
    tryOutConnections();
}


void NodeApp::addOutPeerCandidate(std::string host_in, int port_in, int toTry_in)
{
    PeerCandidateInfo pc = PeerCandidateInfo(host_in, port_in, toTry_in);
    string key = host_in + ":" + to_string(port_in);
    if (myPeerCands.find(key) != myPeerCands.end())
    {
        // already present
        myPeerCands[key].myToTry = toTry_in + myPeerCands[key].myConnTryCount;
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
        cout << "[" << i->first << " " << i->second.myToTry << " " << i->second.myConnTryCount << ":" << i->second.myConnectedCount << "] ";
    }
    cout << endl;
}

void NodeApp::debugPrintPeers()
{
    cout << "Peers: " << myPeers.size() << "  ";
    for (auto i = myPeers.begin(); i != myPeers.end(); ++i)
    {
        cout << "[";
        if (i->myClient == nullptr)
            cout << "n";
        else
            cout << i->myClient->getPeerAddr() << " " << i->myClient->getCanonPeerAddr() << " " << (i->myClient->isConnected() ? "Y" : "N") << "] ";
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
        if (i->second.myConnTryCount == 0 || i->second.myConnTryCount < i->second.myToTry)
        {
            if (!isPeerConnected(i->first, true))
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

bool NodeApp::isPeerConnected(string peerAddr_in, bool outDir_in)
{
    for(auto i = myPeers.begin(); i != myPeers.end(); ++i)
    {
        if (i->myOutFlag == outDir_in)
        {
            if (i->myClient != nullptr)
            {
                if (i->myClient->getPeerAddr() == peerAddr_in || i->myClient->getCanonPeerAddr() == peerAddr_in)
                {
                    return true;
                }
            }
        }
    }
    return false;
}

int NodeApp::tryOutConnection(std::string host_in, int port_in)
{
    // exclude localhost connections to self
    if ((":" + to_string(port_in)) == myName)
    {
        if (host_in == "localhost" || host_in == "127.0.0.1" || host_in == "::1" || host_in == "[::1]")
        {
            //cerr << "Ignoring peer candidate to self (" << host_in << ":" << port_in << ")" << endl;
            return 0;
        }
    }
    // try outgoing connection
    Endpoint ep = Endpoint(host_in, port_in);
    string key = ep.getEndpoint();
    //cout << "Trying outgoing conn to " << key << endl;
    auto peerout = make_shared<PeerClientOut>(this, host_in, port_in);
    auto peerBase = dynamic_pointer_cast<NetClientBase>(peerout);
    PeerInfo p;
    p.setClient(peerBase);
    p.myOutFlag = true;
    myPeers.push_back(p);
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
    PeerInfo p;
    p.setClient(client_in);
    p.myOutFlag = false;
    myPeers.push_back(p);
    //debugPrintPeers();
}

void NodeApp::connectionClosed(NetClientBase* client_in)
{
    assert(client_in != nullptr);
    string cliaddr = client_in->getPeerAddr();
    cout << "App: Connection done: " << cliaddr << " " << myPeers.size() << endl;
    for(auto i = myPeers.begin(); i != myPeers.end(); ++i)
    {
        if (i->myClient != nullptr)
        {
            if (i->myClient.get() == client_in || i->myClient->getPeerAddr() == cliaddr)
            {
                cout << "Removing disconnected client " << myPeers.size() << " " << i->myClient->getPeerAddr() << endl;
                i->resetClient();
            }
        }
    }
    // remove disconnected clients
    bool changed = true;
    while (changed)
    {
        changed = false;
        for(auto i = myPeers.begin(); i != myPeers.end(); ++i)
        {
            if (i->myClient.get() == nullptr)
            {
                myPeers.erase(i);
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
        cout << "App: Received: from " << client_in.getNicePeerAddr() << " '" << msg_in.toString() << "'" << endl;
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

                string peerEp = client_in.getPeerAddr();
                if (!isPeerConnected(peerEp, false))
                {
                    cerr << "Error: cannot find client in peers list " << peerEp << endl;
                    return;
                }

                HandshakeResponseMessage resp("V01", myName, peerEp);
                client_in.sendMessage(resp);

                // find canonical name of this peer: host is actual connected ip, port is reported by peer
                int peerPort = Endpoint(peerEp).getPort();
                string canonHost = Endpoint(peerEp).getHost();
                int canonPort = peerPort;
                string reportedPeerName = hsMsg.getMyAddr();
                if (reportedPeerName.substr(0, 1) != ":")
                {
                    cerr << "Could not retrieve listening port of incoming peer " << client_in.getPeerAddr() << " " << reportedPeerName << endl;
                }
                else
                {
                    canonPort = stoi(reportedPeerName.substr(1));
                    string canonEp = canonHost + ":" + to_string(canonPort);
                    if (canonEp != peerEp)
                    {
                        // canonical is different
                        cout << "Canonical peer of " << peerEp << " is " << canonEp << endl;
                        client_in.setCanonPeerAddr(canonEp);

                        // try to connect ougoing too (to canonical peer addr)
                        addOutPeerCandidate(canonHost, canonPort, 1);
                        tryOutConnections();
                    }
                }
                sendOtherPeers(client_in);
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
                addOutPeerCandidate(peerMsg.getHost(), peerMsg.getPort(), 1);
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
    // collect in a map to discard duplicates
    map<string, int> peers;
    for(auto i = myPeers.begin(); i != myPeers.end(); ++i)
    {
        if (i->myClient != nullptr && i->myClient->isConnected() && i->myClient->getCanonPeerAddr().length() > 0)
        {
            peers[i->myClient->getCanonPeerAddr()] = 1;
        }
    }
    // convert to vector
    vector<Endpoint> vec;
    for(auto i = peers.begin(); i != peers.end(); ++i)
    {
        vec.push_back(Endpoint(i->first));
    }
    return vec;
}
