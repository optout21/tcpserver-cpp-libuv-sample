#pragma once

#include "../lib/net_client.hpp"

#include <uv.h>

#include <string>

namespace sample
{
    class BaseApp; // forward

    class PeerClientOut: public NetClientOut
    {
    public:
        PeerClientOut(BaseApp* app_in, std::string const & host_in, int port_in);
        virtual ~PeerClientOut();
        virtual void process();
        static void on_timer(uv_timer_t* handle);
        virtual void onTimer(uv_timer_t* handle);

    private:
        // Send all known active out peer connections to this peer
        void sendOtherPeers();

    private:
        int mySendCounter;
        uv_timer_t* myTimer;
    };
}
