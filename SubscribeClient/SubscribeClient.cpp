#include "stdafx.h"
#include <CSocketServer.h>
#include <CSocketClient.h>
#include <CMessage.h>
#include <CSocket.h>

using namespace std;

class CDisplayConsole : public CSocketServerDataReceived
{
public:
    inline CDisplayConsole() { count = 0;}
    inline ~CDisplayConsole() {}

    /// <summary>
    /// PubSub subscriber received published data
    /// </summary>
    /// <param name="str"></param>
    inline virtual void DataReceived(std::string str)
    {
        cout << count++ << ": Received: " << str << endl;
    }

    /// <summary>
    /// Callback that indicates to the network layer whether 
    /// it is time to terminate
    /// </summary>
    /// <returns>True if unwind gracefully, False continue</returns>
    inline virtual bool Terminate()
    {
        return bTerminate;
    }

    /// <summary>
    /// Handle Ctrl+C and other exceptions
    /// </summary>
    /// <param name="sigNum"></param>
    inline static void SignalHandler(int sigNum)
    {
        cout << "Caught signal " << sigNum << endl;
        bTerminate = true;
    }

private:
    static bool bTerminate;
    unsigned int count;
};

bool CDisplayConsole::bTerminate = false;

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    cout << "Select type of Subscribe Client" << endl << endl;
    cout << "1.- Subscribe to 'Foo' namespace notifications." << endl;
    cout << "2.- Subscribe to 'Bar' namespace notifications." << endl << endl;
    cout << "Select: ";

    char chSelect;
    cin >> chSelect;

    string strAppName;
    string strPublishNamespace;
    int nListenPort = 0;

    if (chSelect == '1')
    {
        strAppName = "FooApp";
        strPublishNamespace = "AbcApp\\DataProcessed\\Foo";
        nListenPort = 8081;
    }
    else if (chSelect == '2')
    {
        strAppName = "BarApp";
        strPublishNamespace = "AbcApp\\DataProcessed\\Bar";
        nListenPort = 8082;
    }

    if (strAppName.length() > 0)
    {
        WSADATA wsaData;

        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        {
            throw std::runtime_error("WSAStartup Error");
        }

        signal(SIGINT, CDisplayConsole::SignalHandler);

        CDisplayConsole * pConsoleDisplay = new CDisplayConsole();
        CSocketClient* pClient = new CSocketClient();

        try
        {
            CSocketServer server(pConsoleDisplay, nListenPort);

            // Notify PubSub server that we are interested in
            // getting notified of namespace events

            if (pClient->ConnectTo("127.0.0.1", 8080) == true)
            {
                CMessage PublishData(false);

                PublishData.AddData(CMessage::Key::AppName, strAppName);
                PublishData.AddData(CMessage::Key::Namepace, strPublishNamespace);
                PublishData.AddData(CMessage::Key::IpAddress, GetLocalIp());
                PublishData.AddData(CMessage::Key::Port, std::to_string(nListenPort));

                string dataToSend = PublishData.ToJson();

                cout << "Sending: " << dataToSend << endl;

                if (pClient->Send(dataToSend) == true)
                {
                    server.AcceptConnections();
                }
                else
                {
                    cout << "Unable to send what namespace events are of interest." << endl;
                }
            }
            else
            {
                cout << "Unable to subscribe to notification events." << endl;
            }
        }
        catch (char* pMsg)
        {
            cout << "Application exception" << pMsg << endl;
        }
        catch (...)
        {
            cout << "Application exception" << endl;
        }

        delete pConsoleDisplay;
        WSACleanup();
    }

    cout << "Publish Client Terminated\n";
}
