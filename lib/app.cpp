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
    myName = "localhost:" + to_string(actualPort);
}

void ServerApp::stop()
{
    myNetHandler->stop();
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
    int n = 3;
    auto clis = new NetClientBase*[n];
    // create and connect clients
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

    // run the loop, on any of them
    clis[0]->runLoop();
    
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
