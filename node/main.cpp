#include "node.hpp"

#include <iostream>

using namespace sample;
using namespace std;


void usage(AppParams const & params_in)
{
    cout << "TCP LibUV Node" << endl;
    cout << "Usage:  tcp-libuv-node [options]" << endl;
    cout << "  -peer [endpoint]   Extra PeerBoot peer.  Optional.  Example: -peer localhost:5500" << endl;
    cout << "  -port [port]       PeerBoot listening port.  0 for default.  Default: " << params_in.listenPort << endl;
    cout << "Simple example:" << endl;
    cout << "  tcp-libuv-node -port 5005 -peer localhost:5000" << endl;
    cout << endl;
}

void processArgs(AppParams & params_inout, int argn, char ** argc)
{
    params_inout.extraPeers.clear();
    for (int i = 0; i < argn; ++i)
    {
        if (string(argc[i]) == "-peer")
        {
            if (i + 1 >= argn) break;
            ++i;
            params_inout.extraPeers.push_back(argc[i]);
        }
        else if (string(argc[i]) == "-port")
        {
            if (i + 1 >= argn) break;
            ++i;
            params_inout.listenPort = std::stoi(argc[i]);
            params_inout.listenPortRange = 1;  // port is given, only try that one
        }
    }
}

void printArgs(AppParams const & params_in)
{
    if (params_in.extraPeers.size() == 0)
        cout << "Extra peers:     (none)" << endl;
    else
        cout << "Extra peers:     "; for(int i = 0; i < params_in.extraPeers.size(); ++i) cout << params_in.extraPeers[i] << " "; cout << endl;
    cout << "Listening port: " << params_in.listenPort;
    if (params_in.listenPortRange > 1) cout << " (" << params_in.listenPort << " -- " << params_in.listenPort + params_in.listenPortRange - 1 << ")";
    cout << endl;
    cout << endl;
}

int main(int argn, char ** argc)
{
    AppParams appParams(5000, 10);

    usage(appParams);
    processArgs(appParams, argn, argc);
    printArgs(appParams);

    NodeApp app;
    app.start(appParams);

    cout << "Press Enter to exit ...";
    cin.get();
    cout << endl;

    app.stop();
    cout << "Done." << endl;
}
