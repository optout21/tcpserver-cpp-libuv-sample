#include "peer_conn.hpp"

#include <cassert>
#include <iostream>

using namespace sample;
using namespace std;


PeerClientOut::PeerClientOut(BaseApp* app_in, string const & host_in, int port_in) :
NetClientOut(app_in, host_in, port_in, 1),
mySendCounter(0)
{
}

void PeerClientOut::process()
{
    //cout << "PeerClientOut::process " << myState << endl;
    switch (myState)
    {
        case State::Connected:
            {
                mySendCounter = 0;
                HandshakeMessage msg(getNodeAddr());
                sendMessage(msg);
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
            if (mySendCounter >= 1 + 1)
            {
                close();
            }
            else if (mySendCounter >= 1)
            {
                PingMessage msg("Ping_" + getNodeAddr() + "_" + to_string(mySendCounter));
                sendMessage(msg);
            }
            break;

        default:
            cerr << "Fatal error: unhandled state " << myState << endl;
            assert(false);
            break;
    }
    return;
}
