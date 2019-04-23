#include "node.hpp"
#include "peer_conn.hpp"
#include "../lib/net_handler.hpp"
#include "../lib/net_client.hpp"

#include <cassert>
#include <iostream>

using namespace sample;
using namespace std;


NodeApp::PeerInfo::PeerInfo(string endpoint_in) :
myEndpoint(endpoint_in),
myOutClient(nullptr),
myInClient(nullptr)
{
}

NodeApp::PeerInfo::PeerInfo(std::string outHost_in, int outPort_in) :
myEndpoint(outHost_in + ":" + to_string(outPort_in)),
myOutClient(nullptr),
myInClient(nullptr),
myOutHost(outHost_in),
myOutPort(outPort_in),
myStickyOutFlag(true)
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
BaseApp()
{
    myNetHandler = new NetHandler(this);
}

void NodeApp::start()
{
    int actualPort = myNetHandler->startWithListen(5000, 10);
    if (actualPort <= 0) return;
    myName = "localhost:" + to_string(actualPort);
}

void NodeApp::listenStarted(int port)
{
    cout << "App: Listening on port " << port << endl;

    // add sticky bootstrap peers
    int n = 2;
    // try to connect to clients
    for (int i = 0; i < n; ++i)
    {
        int port1 = 5000 + i;
        //clis[i] = nullptr;
        // skip connection to self
        if (port != port1)
        {
            addStickyPeer("localhost", port1);
        }
    }
    tryOutConnections();
}


void NodeApp::addStickyPeer(std::string host_in, int port_in)
{
    PeerInfo peer = PeerInfo(host_in, port_in);
    myPeers[peer.myEndpoint] = peer;
}

void NodeApp::tryOutConnections()
{
    // try to connect to clients
    for (auto i = myPeers.begin(); i != myPeers.end(); ++i)
    {
        if (i->second.myOutClient == nullptr)
        {
            if (i->second.myOutConnTryCount == 0 || i->second.myStickyOutFlag)
            {
                // try outgoing connection
                auto peerout = make_shared<PeerClientOut>(this, i->second.myOutHost, i->second.myOutPort);
                i->second.setOutClient(peerout);
                ++i->second.myOutConnTryCount;
                int res = peerout->connect();
                if (res)
                {
                    // error
                }
            }
        }
    }
}

void NodeApp::stop()
{
    myNetHandler->stop();
}

void NodeApp::inConnectionReceived(std::shared_ptr<NetClientBase>& client_in)
{
    assert(client_in != nullptr);
    string cliaddr = client_in->getNodeAddr();
    cout << "App: New incoming connection: " << cliaddr << endl;
    if (myPeers.find(cliaddr) == myPeers.end())
    {
        myPeers[cliaddr] = PeerInfo(cliaddr);
    }
    auto cliIn = dynamic_pointer_cast<NetClientIn>(client_in);
    myPeers[cliaddr].setInClient(cliIn);
}

void NodeApp::connectionClosed(NetClientBase* client_in)
{
    assert(client_in != nullptr);
    string cliaddr = client_in->getNodeAddr();
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
            if (i->second.myOutClient == nullptr && i->second.myInClient == nullptr && !i->second.myStickyOutFlag)
            {
                cout << "Removing disconnected client " << i->first << endl;
                myPeers.erase(i->first);
                changed = true;
                break;
            }
        }
    } 
}

void NodeApp::messageReceived(NetClientBase & client_in, BaseMessage const & msg_in)
{
    cout << "App: Received: from " << client_in.getNodeAddr() << " '" << msg_in.toString() << "'" << endl;
    switch (msg_in.getType())
    {
        case MessageType::Handshake:
            {
                HandshakeMessage const & hsMsg = dynamic_cast<HandshakeMessage const &>(msg_in);
                //cout << "Handshake message received, '" << hsMsg.getMyAddr() << "'" << endl;
                HandshakeResponseMessage resp(myName, client_in.getNodeAddr());
                client_in.sendMessage(resp);
            }
            break;

        case MessageType::Ping:
            {
                PingMessage const & pingMsg = dynamic_cast<PingMessage const &>(msg_in);
                //cout << "Ping message received, '" << pingMsg.getText() << "'" << endl;
                PingResponseMessage resp("Resp_to_" + pingMsg.getText() + "_from_" + myName);
                client_in.sendMessage(resp);
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


int main()
{
    cout << "TCP LibUV Node" << endl;

    NodeApp app;
    app.start();

    cout << "Press Enter to exit ...";
    cin.get();
    cout << endl;

    app.stop();
    cout << "Done." << endl;
}
