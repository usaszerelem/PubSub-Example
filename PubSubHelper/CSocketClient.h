#pragma once

class CSocketClient
{
public:
    CSocketClient();
   ~CSocketClient();

    bool ConnectTo(std::string strIpAddress, int nServerListenPort);
    bool Send(std::string strData);

private:
    int m_Socket;
};

