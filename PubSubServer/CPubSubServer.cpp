#include "stdafx.h"
#include <assert.h>
#include <CSocketServer.h>
#include <CMessage.h>
#include "CPubSubServer.h"
#include <CSocketClient.h>

using namespace std;

static bool bTerminate = false;

// ---------------------------------------------------------------------------
// Define the function to be called when ctrl-c (SIGINT) is sent to process
// ---------------------------------------------------------------------------

void CPubSubServer::SignalHandler(int signum) {
    cout << "Caught signal " << signum << endl;
    bTerminate = true;
}

CPubSubServer::CPubSubServer()
{
}

CPubSubServer::~CPubSubServer()
{
    for (std::map<string, CMessage*>::iterator it = m_Subscribers.begin(); it != m_Subscribers.end(); ++it)
    {
        delete it->second;
    }

    m_Subscribers.clear();
}

void CPubSubServer::DataReceived(std::string strJson)
{
    CMessage data(strJson);
    string sNamespace = data.FindData(CMessage::Key::Namepace);

    if (data.IsPublish() == true)
    {
        cout << "Received data to publish. Forwarding it to subscribers: " << endl << strJson << endl;

        // See if there is somebody interested in this published data
        // Start by iterating through all subscribers and see if there
        // is a match on the namespace

        for (std::multimap<string, CMessage*>::iterator it = m_Subscribers.begin(); it != m_Subscribers.end();)
        {
            string theNamespaceOfInterest = it->first;

            // Check if the namespace for the data received is a substring of
            // the subscriber's namespace of interest and that the substring
            // starts at position 0

            size_t pos = sNamespace.find(theNamespaceOfInterest);

            if (pos == 0)
            {
                // Match was found. There coud be several subscribers to the same

                auto keyRange = m_Subscribers.equal_range(sNamespace);

                for (auto i = keyRange.first; i != keyRange.second;)
                {
                    // Get URL and Port where to send the published message
                    pair<string, int> connectInfo = GetSubscriberConnectInfo(it->second);

                    CSocketClient subscriber;
                    bool success = false;

                    if (subscriber.ConnectTo(connectInfo.first, connectInfo.second) == true)
                    {
                        success = subscriber.Send(strJson);
                        assert(success == true); // How come if fails?
                    }

                    if (success == false)
                    {
                        i = m_Subscribers.erase(i);
                    }
                    else
                    {
                        i++;
                    }
                }

                it = keyRange.second;
            }
            else
            {
                it++;
            }
        }
    }
    else
    {
        cout << "Subscription received : " << endl << strJson << endl;
        // A process is interested in subscribing to a namespace
        // m_Publishers[sNamespace] = new CMessage(data);
        m_Subscribers.insert(make_pair(sNamespace, new CMessage(data)));
    }
}

pair<string, int> CPubSubServer::GetSubscriberConnectInfo(CMessage * pData)
{
    string strIp = pData->FindData(CMessage::Key::IpAddress);
    string strPort = pData->FindData(CMessage::Key::Port);

    assert(strIp.length() > 0);
    assert(strPort.length() > 0);

    int nPort = std::stoi(strPort);

    pair<string, int> connectInfo(strIp, nPort);

    return connectInfo;
}

bool CPubSubServer::Terminate()
{
    return bTerminate;
}
