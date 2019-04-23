#include "node.hpp"
#include "peer_conn.hpp"
#include "../lib/net_handler.hpp"
#include "../lib/net_client.hpp"

#include <cassert>
#include <iostream>

using namespace sample;
using namespace std;


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

    // create and connect clients
    int n = 3;
    auto clis = new NetClientBase*[n];
    // try to connect to clients
    for (int i = 0; i < n; ++i)
    {
        int port1 = 5000 + i;
        clis[i] = nullptr;
        // skip connection to self
        if (port != port1)
        {
            auto nc = new PeerClientOut(this, "localhost", port1);
            clis[i] = nc;
            int res = nc->connect();
            if (res)
            {
                // error
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
    myClients[cliaddr] = client_in;
}

void NodeApp::connectionClosed(std::shared_ptr<NetClientBase>& client_in)
{
    assert(client_in != nullptr);
    string cliaddr = client_in->getNodeAddr();
    cout << "App: Connection done: " << cliaddr << endl;
    for(auto i = myClients.begin(); i != myClients.end(); ++i)
    {
        if (i->second.get() == client_in.get())
        {
            myClients.erase(i->first);
            break;
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
