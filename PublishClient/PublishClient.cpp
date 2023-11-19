#include "stdafx.h"
#include "CPublishThread.h"
#include <CmdLine.h>

using namespace std;

static const char* pszKeyAppName = "appname";
static const char* pszKeyNamespace = "namespace";
static const char* pszKeyPublishIp = "ip";
static const char* pszKeyPublishPort = "port";

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void DisplaySyntax()
{
    cout << "Missing command line parameters. Correct syntax:" << endl << endl;
    cout << "PublishClient.exe -" << pszKeyAppName << "=PublishClient ";
    cout << "-" << pszKeyNamespace << "=AbcApp\\DataProcessed\\Foo ";
    cout << "-" << pszKeyPublishIp << "=127.0.0.1 ";
    cout << "-" << pszKeyPublishPort << "=8080" << endl << endl;
}

/// <summary>
/// Get the parameters needed to publish events to a namespace
/// </summary>
/// <returns>Retreived paramaters in CPublishParam</returns>

CPublishParam GetInputParams()
{
    CPublishParam params;

    cout << "App name. [Enter for 'AbcApp']  ";
    getline(std::cin, params.m_AppName);

    if (params.m_AppName.length() == 0)
    {
        params.m_AppName = "PublishClient";
    }

    cout << "Namespace. [Enter for 'AbcApp\\DataProcessed\\Foo']: ";
    getline(std::cin, params.m_Namespace);

    if (params.m_Namespace.length() == 0)
    {
        params.m_Namespace = "AbcApp\\DataProcessed\\Foo";
    }

    cout << "Publish IP. [Enter for loopback]: ";
    getline(std::cin, params.m_PubSubServerIp);

    if (params.m_PubSubServerIp.length() == 0)
    {
        params.m_PubSubServerIp = "127.0.0.1";
    }

    string strPort;
    cout << "Port. [Enter for default port 8080]: ";
    getline(std::cin, strPort);

    if (strPort.length() == 0)
    {
        params.m_PubSubServerPort = 8080;
    }
    else
    {
        params.m_PubSubServerPort = stoi(strPort);
    }

    return params;
}

/// <summary>
/// Get the command line arguments for this publish client.
/// </summary>
/// <param name="argc"></param>
/// <param name="argv"></param>
/// <returns>Retreived paramaters in CPublishParam</returns>

CPublishParam GetCmdLineParams(int argc, char* argv[])
{
    CmdLine cmdLine(argc, argv);
    CPublishParam params;

    params.m_AppName = cmdLine.Find(pszKeyAppName);
    params.m_Namespace = cmdLine.Find(pszKeyNamespace);
    params.m_PubSubServerIp = cmdLine.Find(pszKeyPublishIp);

    string s = cmdLine.Find(pszKeyPublishPort);
    params.m_PubSubServerPort = stoi(s.length() == 0 ? "0" : s);

    return params;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    CPublishParam params = GetCmdLineParams(argc, argv);

    if (params.HasAllParams() == false)
    {
        DisplaySyntax();
        params = GetInputParams();
    }

    assert(params.HasAllParams() == true);

    try
    {
        WSADATA wsaData;

        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        {
            throw std::runtime_error("WSAStartup Error");
        }

        CPublishThread Publish(params);
        cout << "Publish Thread started\n";
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

    cout << "Publish Client Terminated\n";
}
