#include "stdafx.h"
#include "CPublishThread.h"

using namespace std;

static bool bTerminate = false;

// ---------------------------------------------------------------------------
// Define the function to be called when ctrl-c (SIGINT) is sent to process
// ---------------------------------------------------------------------------

void signal_callback_handler(int signum) {
    cout << "Caught signal " << signum << endl;
    bTerminate = true;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    cout << "Select type of Publish Client" << endl << endl;
    cout << "1.- Publish to 'Foo' namespace." << endl;
    cout << "2.- Publish to 'Bar' namespace." << endl << endl;
    cout << "Select: ";

    char chSelect;
    cin >> chSelect;

    string strAppName;
    string strPublishNamespace;

    if (chSelect == '1')
    {
        strAppName = "FooApp";
        strPublishNamespace = "AbcApp\\DataProcessed\\Foo";
    }
    else if (chSelect == '2')
    {
        strAppName = "BarApp";
        strPublishNamespace = "AbcApp\\DataProcessed\\Bar";
    }

    if (strAppName.length() > 0)
    {
        signal(SIGINT, signal_callback_handler);

        try
        {
            WSADATA wsaData;

            if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
            {
                throw std::runtime_error("WSAStartup Error");
            }

            CPublishThread Publish(strAppName, strPublishNamespace);

            if (Publish.Init(&bTerminate) == true)
            {
                cout << "Publish Thread started\n";
            }
        }
        catch (char* pMsg)
        {
            cout << "Publish client error: " << pMsg << endl;
        }
        catch (...)
        {
            cout << "Publish client error";
        }

        WSACleanup();
    }

    cout << "Publish Client Terminated\n";
}
