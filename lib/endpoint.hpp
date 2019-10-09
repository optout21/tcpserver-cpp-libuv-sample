#pragma once

#include <string>

namespace sample
{
    /**
     * Endpoint handling.
     */
    class Endpoint
    {
    public:
        /// Construct from string
        Endpoint() = default;
        Endpoint(std::string endpointStr_in);
        Endpoint(std::string host_in, int port_in);
        std::string getHost() const { return myHost; }
        int getPort() const { return myPort; }
        std::string getEndpoint() const { return myHost + ":" + std::to_string(myPort); }
    private:
        std::string myHost;
        int myPort;
    };
}
