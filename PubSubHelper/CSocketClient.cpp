#include "pch.h"
#include "CSocketClient.h"

using namespace std;

CSocketClient::CSocketClient()
    : m_Socket(-1)
{
}

CSocketClient::~CSocketClient()
{
    if (m_Socket < 0)
    {
        closesocket(m_Socket);
    }
}

bool CSocketClient::ConnectTo(std::string strIpAddress, int nServerListenPort)
{
    struct sockaddr_in serv_addr;

    if (m_Socket < 0)
    {
        m_Socket = (int)socket(AF_INET, SOCK_STREAM, 0);

        if (m_Socket < 0)
        {
            throw std::runtime_error("Socket creation error");
        }

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(nServerListenPort);

        // Convert IPv4 and IPv6 addresses from text to binary form
        if (inet_pton(AF_INET, strIpAddress.c_str(), &serv_addr.sin_addr) <= 0)
        {
            closesocket(m_Socket);
            m_Socket = -1;
            throw std::runtime_error("Invalid address - Address not supported");
        }

        int status = connect(m_Socket, (struct sockaddr*)&serv_addr,
            sizeof(serv_addr));

        if (status < 0)
        {
            closesocket(m_Socket);
            m_Socket = -1;
            cout << "Connection Failed. Nobody home..." << endl;
        }
    }

    return(m_Socket < 0 ? false : true);
}

bool CSocketClient::Send(std::string strData)
{
    assert(m_Socket >= 0);

    if (m_Socket >= 0)
    {
#pragma warning(disable:4267) // conversion from size_t to int

        int bytesSent = send(m_Socket, strData.c_str(), strData.length(), 0);

        if (bytesSent != strData.length())
        {
            cout << "Socket did not send all data." << endl;
            closesocket(m_Socket);
            m_Socket = -1;
        }
    }

    return(m_Socket >= 0 ? true : false);
}