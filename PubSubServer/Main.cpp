#include "stdafx.h"
#include <signal.h>
#include "CSocketServer.h"
#include "CPubSubServer.h"

int main()
{
    cout << "PubSubServer creates a server connection and waits for" << endl;
    cout << "namespace subscription requests. These requests include the IP address" << endl;
    cout << "to be notified as well as the namespace of interest." << endl << endl;
    cout << "The server also waits for events originating from namespaces that." << endl;
    cout << "publish events into them." << endl << endl;
    cout << "These events are forwarded to listeners for these namespace events." << endl << endl;

    CPubSubServer* pPubSub = nullptr;

    try
    {
        WSADATA wsaData;

        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        {
            throw std::runtime_error("WSAStartup Error");
        }

        pPubSub = new CPubSubServer();
        signal(SIGINT, CPubSubServer::SignalHandler);

        CSocketServer server(pPubSub, 8080);
        server.AcceptConnections();
    }
    catch (char* pMsg)
    {
        cout << pMsg << endl;
    }
    catch (...)
    {
        cout << "Application error";
    }

    delete pPubSub;
    WSACleanup();
}
