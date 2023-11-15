#pragma once

#include "CMessage.h"

class CPubSubServer : public CSocketServerDataReceived
{
public:
    CPubSubServer();
    ~CPubSubServer();

    static void SignalHandler(int signum);
    void DataReceived(std::string strJson);
    bool Terminate();

private:
    static pair<string, int> GetSubscriberConnectInfo(CMessage* pData);
    std::multimap<string, CMessage*> m_Subscribers;
};
