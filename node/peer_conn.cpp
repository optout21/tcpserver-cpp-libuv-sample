#include "peer_conn.hpp"
#include "../lib/net_handler.hpp"
#include "../lib/app.hpp"
#include "node.hpp"

#include <cassert>
#include <iostream>

using namespace sample;
using namespace std;

PeerClientOut::PeerClientOut(BaseApp* app_in, string const & host_in, int port_in) :
NetClientOut(app_in, host_in, port_in, 1),
mySendCounter(0),
myTimer(nullptr)
{
}

PeerClientOut::~PeerClientOut()
{
    //cout << "PeerClientOut::~PeerClientOut" << endl;
    if (myTimer != nullptr)
    {
        uv_timer_stop(myTimer);
        myTimer = nullptr;
    }
}

void PeerClientOut::on_timer(uv_timer_t* handle)
{
    IUvSocket* uvSocket = (IUvSocket*)(handle->data);
    //cout << "on_timer " << (long)handle << " " << (long)uvSocket << endl;
    if (uvSocket == nullptr)
    {
        cerr << "Fatal error: uvSocket is nullptr " << endl;
        //uv_close((uv_handle_t*)req->handle, NULL);
        return;
    }
    uvSocket->onTimer(handle);
}

void PeerClientOut::onTimer(uv_timer_t* handle)
{
    //cout << "onTimer " << myState << " " << isConnected() << " " << (long)handle << endl;
    PingMessage msg("Ping_from_" + myApp->getName() + "_to_" + getPeerAddr() + "_" + to_string(mySendCounter));
    sendMessage(msg);
    ((NodeApp*)myApp)->sendOtherPeers(*(dynamic_cast<NetClientBase*>(this)));
}

void PeerClientOut::process()
{
    //cout << "PeerClientOut::process " << myState << endl;
    switch (myState)
    {
        case State::Connected:
            {
                myTimer = new uv_timer_t();
                uv_timer_init(NetHandler::getUvLoop(), myTimer);
                int pingPeriod = 3000; // ms
                this->onTimer(nullptr);
                myTimer->data = (void*)this;
                //timer->data = (void*)dynamic_cast<IUvSocket*>(this);
                uv_timer_start(myTimer, PeerClientOut::on_timer, pingPeriod, pingPeriod);
                mySendCounter = 0;
                HandshakeMessage msg("V01", getPeerAddr(), myApp->getName());
                sendMessage(msg);
                ((NodeApp*)myApp)->sendOtherPeers(*(dynamic_cast<NetClientBase*>(this)));
            }
            break;

        case State::Sending:
            ++mySendCounter;
            doRead();
            break;

        case State::Sent:
            doRead();
            break;

        case State::Receiving:
            doRead();
            break;

        case State::Received:
            break;

        default:
            cerr << "Fatal error: unhandled state " << myState << endl;
            assert(false);
            break;
    }
    return;
}
