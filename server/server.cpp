#include "../lib/app.hpp"

#include <iostream>

using namespace sample;
using namespace std;

int main()
{
    cout << "TCP LibUV Server" << endl;

    ServerApp app;
    app.start(AppParams(5000, 5));

    cout << "Press Enter to exit ...";
    cin.get();
    cout << endl;

    app.stop();
    cout << "Done." << endl;
}
