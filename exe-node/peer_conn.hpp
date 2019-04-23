#pragma once

#include "../lib/net_client.hpp"

#include <string>

namespace sample
{
    class BaseApp; // forward

    class PeerClientOut: public NetClientOut
    {
    public:
        PeerClientOut(BaseApp* app_in, std::string const & host_in, int port_in);
        virtual void process();

    private:
        int mySendCounter;
    };
}
