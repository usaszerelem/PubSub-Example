#include "stdafx.h"
#include <CSocketServer.h>
#include <CSocketClient.h>
#include <CMessage.h>
#include <CSocket.h>
#include <CmdLine.h>
#include <cassert>

using namespace std;

/// <summary>
/// Command line argument key values
/// </summary>

const char* pszKeyAppName = "appname";
const char* pszKeyNamespace = "namespace";
const char* pszKeyPubSubIp = "pubsubip";
const char* pszKeyPubSubPort = "pubsubport";
const char* pszKeyListenOnIp = "listenip";
const char* pszKeyListenOnPort = "listenport";

/// <summary>
/// Displays received pubsub notifications
/// </summary>

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

/// <summary>
/// Parameters that are need for this client to establish connection with the
/// pubsub server and listen to messages coming from it.
/// </summary>

class CSubscribeParams
{
    public:
        inline CSubscribeParams()
            : m_AppName(), m_Namespace(), m_PubSubServerIp(), m_PubSubServerPort(0),
              m_ListenOnIp(), m_ListedOnPort(0)
        {}

        inline CSubscribeParams operator =(const CSubscribeParams src)
        {
            m_AppName = src.m_AppName;
            m_Namespace = src.m_Namespace;
            m_PubSubServerIp = src.m_PubSubServerIp;
            m_PubSubServerPort = src.m_PubSubServerPort;
            m_ListenOnIp = src.m_ListenOnIp;
            m_ListedOnPort = src.m_ListedOnPort;

            return *this;
        }

        inline bool HasAllParams()
        {
            if (m_AppName.length() > 0 && m_Namespace.length() > 0 &&
                m_PubSubServerIp.length() > 0 && m_ListenOnIp.length() > 0 &&
                m_PubSubServerPort > 0 && m_ListedOnPort > 0)
            {
                return true;
            }

            return false;
        }

    public:
        string m_AppName;
        string m_Namespace;
        string m_PubSubServerIp;
        int    m_PubSubServerPort;
        string m_ListenOnIp;
        int    m_ListedOnPort;
};

/// <summary>
/// Collect parameters that are need for this client to establish connection with the
/// pubsub server and listen to messages coming from it.
/// </summary>
/// <returns>Initialized CSubscribeParams</returns>

CSubscribeParams GetInputParams()
{
    CSubscribeParams params;

    cout << "App name. [Enter for 'SubscribeClient']  ";
    getline(std::cin, params.m_AppName);

    if (params.m_AppName.length() == 0)
    {
        params.m_AppName = "SubscribeClient";
    }

    cout << "Namespace. [Enter for 'AbcApp\\DataProcessed\\Foo']: ";
    getline(std::cin, params.m_Namespace);

    if (params.m_Namespace.length() == 0)
    {
        params.m_Namespace = "AbcApp\\DataProcessed\\Foo";
    }

    cout << "PubSub Server IP. [Enter for loopback]: ";
    getline(std::cin, params.m_PubSubServerIp);

    if (params.m_PubSubServerIp.length() == 0)
    {
        params.m_PubSubServerIp = "127.0.0.1";
    }

    string strPort;
    cout << "PubSub Server Port. [Enter for default port 8080]: ";
    getline(std::cin, strPort);

    if (strPort.length() == 0)
    {
        params.m_PubSubServerPort = 8080;
    }
    else
    {
        params.m_PubSubServerPort = stoi(strPort);
    }

    cout << "List on IP. [Enter for loopback]: ";
    getline(std::cin, params.m_ListenOnIp);

    if (params.m_ListenOnIp.length() == 0)
    {
        params.m_ListenOnIp = "127.0.0.1";
    }

    cout << "Listen on Port. [Enter for default port 8081]: ";
    getline(std::cin, strPort);

    if (strPort.length() == 0)
    {
        params.m_ListedOnPort = 8081;
    }
    else
    {
        params.m_ListedOnPort = stoi(strPort);
    }

    return params;
}

CSubscribeParams GetCmdLineArgs(int argc, char* argv[])
{
    CSubscribeParams params;
    CmdLine cmdLine(argc, argv);

    params.m_AppName = cmdLine.Find(pszKeyAppName);
    params.m_Namespace = cmdLine.Find(pszKeyNamespace);
    params.m_PubSubServerIp = cmdLine.Find(pszKeyPubSubIp);

    string s = cmdLine.Find(pszKeyPubSubPort);
    params.m_PubSubServerPort = stoi(s.length() == 0 ? "0" : s);

    params.m_ListenOnIp = cmdLine.Find(pszKeyListenOnIp);
    s = cmdLine.Find(pszKeyListenOnPort);

    params.m_ListedOnPort = stoi(s.length() == 0 ? "0" : s);

    return params;
}

/// <summary>
/// Display how to use this client syntax
/// </summary>
void DisplaySyntax()
{
    cout << "Missing command line parameters. Correct syntax:" << endl << endl;
    cout << "SubscribeClient.exe -" << pszKeyAppName << "=SubscibeClient ";
    cout << "-" << pszKeyNamespace << "=AbcApp\\DataProcessed\\Foo ";
    cout << "-" << pszKeyPubSubIp << "=127.0.0.1 ";
    cout << "-" << pszKeyPubSubPort << "=8080";
    cout << "-" << pszKeyListenOnIp << "=127.0.0.1 ";
    cout << "-" << pszKeyListenOnPort << "=8081" << endl << endl;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    CSubscribeParams params = GetCmdLineArgs(argc, argv);

    if (params.HasAllParams() == false)
    {
        DisplaySyntax();
        params = GetInputParams();
    }

    assert(params.HasAllParams() == true);

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
        CSocketServer server(pConsoleDisplay, params.m_ListenOnIp, params.m_ListedOnPort);

        // Notify PubSub server that we are interested in
        // getting notified of namespace events

        if (pClient->ConnectTo("127.0.0.1", 8080) == true)
        {
            CMessage PublishData(false);

            PublishData.AddData(CMessage::Key::AppName, params.m_AppName);
            PublishData.AddData(CMessage::Key::Namepace, params.m_Namespace);
            PublishData.AddData(CMessage::Key::IpAddress, params.m_ListenOnIp);
            PublishData.AddData(CMessage::Key::Port, std::to_string(params.m_ListedOnPort));

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

    cout << "Publish Client Terminated\n";
}
