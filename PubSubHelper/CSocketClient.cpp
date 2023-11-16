#include "pch.h"
#include "CSocketClient.h"

using namespace std;

/// <summary>
/// Initializes a simple socket client to send messages
/// As we all know, the socket libraries are not the most
/// type safe.
/// </summary>
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

/// <summary>
/// Attemps to make a connection to a listening processes that should
/// be on the provided IP address and Port
/// </summary>
/// <param name="strIpAddress"></param>
/// <param name="nServerListenPort"></param>
/// <returns></returns>
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

/// <summary>
/// Send the provied string via the connected socket. If send fails
/// the socket is closed.
/// </summary>
/// <param name="strData">Data to send</param>
/// <returns>Boolean True if information was sent.</returns>
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
