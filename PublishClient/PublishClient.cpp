#include "stdafx.h"
#include "CPublishThread.h"
#include <CmdLine.h>

using namespace std;

static const char* pszKeyPublishName = "appname";
static const char* pszKeyPublishNamespace = "namespace";
static const char* pszKeyPublishUrl = "url";
static const char* pszKeyPublishPort = "port";

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void DisplaySyntax()
{
    cout << "Missing command line parameters. Correct syntax:" << endl << endl;
    cout << "PublishClient.exe -" << pszKeyPublishName << "=AbcApp ";
    cout << "-" << pszKeyPublishNamespace << "=AbcApp\\DataProcessed\\Foo ";
    cout << "-" << pszKeyPublishUrl << "=127.0.0.1 ";
    cout << "-" << pszKeyPublishPort << "=8080" << endl << endl;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

CPublishParam GetInputParams()
{
    CPublishParam params;

    cout << "App name. [Enter for 'AbcApp']  ";
    getline(std::cin, params.m_publishName);

    if (params.m_publishName.length() == 0)
    {
        params.m_publishName = "AbcApp";
    }

    cout << "Namespace. [Enter for 'AbcApp\\DataProcessed\\Foo']: ";
    getline(std::cin, params.m_publishNamespace);

    if (params.m_publishNamespace.length() == 0)
    {
        params.m_publishNamespace = "AbcApp\\DataProcessed\\Foo";
    }

    cout << "Publish URL. [Enter for loopback]: ";
    getline(std::cin, params.m_publishUrl);

    if (params.m_publishUrl.length() == 0)
    {
        params.m_publishUrl = "127.0.0.1";
    }

    string strPort;
    cout << "Port. [Enter for default port 8080]: ";
    getline(std::cin, strPort);

    if (strPort.length() == 0)
    {
        params.m_publishPort = 8080;
    }
    else
    {
        params.m_publishPort = stoi(strPort);
    }

    return params;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    CmdLine cmdLine(argc, argv);

    CPublishParam params;

    params.m_publishName = cmdLine.Find(pszKeyPublishName);
    params.m_publishNamespace = cmdLine.Find(pszKeyPublishName);
    params.m_publishUrl = cmdLine.Find(pszKeyPublishName);

    string s = cmdLine.Find(pszKeyPublishName);
    params.m_publishPort = stoi(s.length() == 0 ? "0" : s);

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
