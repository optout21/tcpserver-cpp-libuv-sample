#include "app.hpp"

#include "net_handler.hpp"
#include "net_client.hpp"
#include "message.hpp"
#include "uv_socket.hpp"

#include <cassert>
#include <iostream>
#include <memory>

using namespace sample;
using namespace std;


void BaseApp::listenStarted(int port)
{
    cout << "App: Listening on port " << port << endl;
}


ServerApp::ServerApp() :
BaseApp()
{
    myNetHandler = new NetHandler(this);
}

void ServerApp::start(AppParams const & appParams_in)
{
    int actualPort = myNetHandler->startWithListen(appParams_in.listenPort, appParams_in.listenPortRange);
    if (actualPort <= 0)
    {
        return;
    }
    myName = ":" + to_string(actualPort);
}

void ServerApp::stop()
{
    myNetHandler->stop();
}

void ServerApp::inConnectionReceived(shared_ptr<NetClientBase>& client_in)
{
    assert(client_in != nullptr);
    string cliaddr = client_in->getNicePeerAddr();
    cout << "App: New incoming connection: " << cliaddr << endl;
    myClients[cliaddr] = client_in;
}

void ServerApp::connectionClosed(NetClientBase* client_in)
{
    assert(client_in != nullptr);
    string cliaddr = client_in->getPeerAddr();
    cout << "App: Connection done: " << cliaddr << endl;
    for(auto i = myClients.begin(); i != myClients.end(); ++i)
    {
        if (i->second.get() == client_in)
        {
            myClients.erase(i->first);
            break;
        }
    }
}

void ServerApp::messageReceived(NetClientBase & client_in, BaseMessage const & msg_in)
{
    cout << "App: Received: from " << client_in.getNicePeerAddr() << " '" << msg_in.toString() << "'" << endl;
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

        default:
            assert(false);
    }
}


ClientApp::ClientApp() :
BaseApp()
{
}

void ClientApp::start(AppParams const & appParams_in)
{
    // create and connect clients, before starting the loop
    int n = 3;
    auto clis = new NetClientBase*[n];
    for (int i = 0; i < n; ++i)
    {
        auto nc = new NetClientOut(this, "localhost", appParams_in.listenPort + i, 3 + i);
        clis[i] = nc;
        int res = nc->connect();
        if (res)
        {
            // error
        }
    }

    // run the loop
    NetHandler::runLoop();
    
    // if loop finishes, all are done
    // delete clients
    for (int i = 0; i < n; ++i)
    {
        delete clis[i];
        clis[i] = nullptr;
    }
}

void ClientApp::messageReceived(NetClientBase & client_in, BaseMessage const & msg_in)
{
    cout << "App: Received: from " << client_in.getPeerAddr() << " '" << msg_in.toString() << "'" << endl;
}
