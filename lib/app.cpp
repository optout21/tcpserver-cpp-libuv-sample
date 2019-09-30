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

void ServerApp::start()
{
    int actualPort = myNetHandler->startWithListen(5000, 10);
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
    string cliaddr = client_in->getNodeAddr();
    cout << "App: New incoming connection: " << cliaddr << endl;
    myClients[cliaddr] = client_in;
}

void ServerApp::connectionClosed(NetClientBase* client_in)
{
    assert(client_in != nullptr);
    string cliaddr = client_in->getNodeAddr();
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
    cout << "App: Received: from " << client_in.getNodeAddr() << " '" << msg_in.toString() << "'" << endl;
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
                HandshakeResponseMessage resp("V01", myName, client_in.getNodeAddr());
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

        default:
            assert(false);
    }
}


ClientApp::ClientApp() :
BaseApp()
{
}

void ClientApp::start()
{
    // create and connect clients, before starting the loop
    int n = 3;
    auto clis = new NetClientBase*[n];
    for (int i = 0; i < n; ++i)
    {
        auto nc = new NetClientOut(this, "localhost", 5000 + i, 3 + i);
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
    cout << "App: Received: from " << client_in.getNodeAddr() << " '" << msg_in.toString() << "'" << endl;
}
