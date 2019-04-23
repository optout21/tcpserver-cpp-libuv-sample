#include "../lib/app.hpp"

#include <iostream>

using namespace sample;
using namespace std;

int main()
{
    cout << "TCP LibUV Client" << endl;

    ClientApp app;
    app.start();

    //cout << "Press Enter to exit ...";
    //cin.get();
    //cout << endl;

    cout << "Done." << endl;
}
