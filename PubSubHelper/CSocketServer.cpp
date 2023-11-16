#include "pch.h"
#include "CSocketServer.h"
#include "CSocketClient.h"
#include "CSocket.h"

using namespace std;

/// <summary>
/// Initialize a socket server connection.
/// </summary>
/// <param name="pCallback">Implemented callback interfaces to notify of messages</param>
/// <param name="ListenPort">Port number to start listening on.</param>

CSocketServer::CSocketServer(CSocketServerDataReceived* pCallback, int ListenPort)
    : m_Socket(-1), m_Mutex(), m_Connections(), m_pCallback(pCallback)
{
    Initialize(ListenPort);
}

/// <summary>
/// Frees up all allocated resources and disconnect any connected clients
/// </summary>
CSocketServer::~CSocketServer()
{
    for (std::map<int, CThreadParams*>::iterator it = m_Connections.begin(); it != m_Connections.end(); ++it)
    {
        CThreadParams* pTParams = it->second;

        if (pTParams->worker.joinable() == true)
        {
            pTParams->bTerminate = true;
            pTParams->worker.join();
            delete pTParams;
        }
    }

    m_Connections.clear();
}

/// <summary>
/// Initialize the server socket and configure it for non-blocking
/// behavior. When all done start listening on the specified port.
/// </summary>
/// <param name="ListenPort">Port number to listen on.</param>
void CSocketServer::Initialize(int ListenPort)
{
    if (m_Socket >= 0)
    {
        closesocket(m_Socket);
    }

    m_Socket = (int)socket(AF_INET, SOCK_STREAM, 0);

    if (m_Socket < 0)
    {
        throw std::runtime_error("Socket creation error");
    }

    const char opt = 1;

    if (setsockopt(m_Socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        closesocket(m_Socket);
        m_Socket = -1;
        throw std::runtime_error("Socket setsockopt error");
    }

    //-------------------------
    // Set the socket I/O mode: In this case FIONBIO enables or disables the
    //  blocking mode for the socket based on the numerical value of ulBlocking.
    // If ulBlocking = 0, blocking is enabled; 
    // If ulBlocking != 0, non-blocking mode is enabled.

    u_long ulBlocking = 1;
    if (ioctlsocket(m_Socket, FIONBIO, &ulBlocking) != NO_ERROR)
    {
        closesocket(m_Socket);
        m_Socket = -1;
        throw std::runtime_error("Socket ioctlsocket failed");
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(ListenPort);

    // Attaching socket to the specified port
    if (bind(m_Socket, (struct sockaddr*)&address, sizeof(address)) == SOCKET_ERROR)
    {
        closesocket(m_Socket);
        m_Socket = -1;
        throw std::runtime_error("Socket Bind error");
    }

    string strIp = GetLocalIp();

    cout << "Server is listening on: " << strIp << ":" << ListenPort << endl;

    // Defines the maximum length for the queue of pending connections.
    const int backlog = 5;

    if (listen(m_Socket, backlog) == SOCKET_ERROR)
    {
        closesocket(m_Socket);
        m_Socket = -1;
        throw std::runtime_error("Socket Listen error");
    }
}

/// <summary>
/// Now that the server socket is initialized, it is ready to accept connections.
/// </summary>
void CSocketServer::AcceptConnections()
{
    int newSocket;
    CSocketClient* pPubSub = new CSocketClient();

    std::cout << "Waiting for client connections" << endl;

    do
    {
        struct sockaddr addressNew;
        socklen_t addrlen = sizeof(addressNew);

        newSocket = (int)accept(m_Socket, &addressNew, &addrlen);

        if (newSocket >= 0)
        {
            cout << endl << "New connection request received." << endl;

            // Create thread specific data so that the new connection
            // has a dedicated listening thread to accept messages.

            CThreadParams* pTData = new CThreadParams();
            pTData->Socket = newSocket;
            pTData->pCallback = m_pCallback;
            pTData->bTerminate = false;
            pTData->pSocketServer = this;
            pTData->worker = thread (&CSocketServer::ThreadWorker, pTData);

            m_Mutex.lock();
            m_Connections[newSocket] = pTData;
            m_Mutex.unlock();
        }

        // Wake up every n seconds and check if there is a connection request.
        this_thread::sleep_for(chrono::milliseconds(1000ms));

    } while (m_pCallback->Terminate() == false);

    closesocket(m_Socket);
    m_Socket = -1;
    cout << endl << "Socket server stopped listening" << endl;
}

/// <summary>
/// Listen for data to arrive.
/// </summary>
/// <param name="pParams"></param>
void CSocketServer::ThreadWorker(CThreadParams* pParams)
{
    bool bConnected = true;

    // We should be connected and process terminate and class terminate
    // should be false.

    while (bConnected == true && 
        pParams->pCallback->Terminate() == false &&
        pParams->bTerminate == false)
    {
        try
        {
            std::string strReceived;
            
            bConnected = ReadReceivedData(pParams->Socket, strReceived);

            if (strReceived.length() > 0)
            {
                pParams->pCallback->DataReceived(strReceived);
            }
            else if (bConnected == false)
            {
                cout << "Connection closed." << endl;
                break;
            }            
        }
        catch (char* pMsg)
        {
            cout << pMsg;
            break;
        }
        catch (...)
        {
            cout << "Internal Error CSocketServer";
            break;
        }
    }

    pParams->pSocketServer->ThreadTerminated(pParams->Socket);
}

/// <summary>
/// Read the socket stream while there is data
/// </summary>
/// <param name="Socket">Socket that is open</param>
/// <param name="strRet">Return read data here</param>
/// <returns>Boolean True if connection is alive, False otherwise</returns>
bool CSocketServer::ReadReceivedData(int Socket, string &strRet)
{
    const int BuffSize = 1025;
    char buffer[BuffSize] = { 0 };
    int bytesReceived = 0;
    bool bConnectionAlive = false;

    strRet.clear();

    do
    {
        int recVal = 0;

        // Use select to wait for a time period for data to arrive.
        // This is only of interest if number of bytes received is
        // maximum that we requested to be read.

        if (bytesReceived != (BuffSize - 1))
        {
            struct timeval tv;
            memset(&tv, '\0', sizeof(tv));
            tv.tv_sec = 1;
            tv.tv_usec = 0;

            // File descriptor for reading
            fd_set fdread;
            FD_ZERO(&fdread);
            FD_SET(Socket, &fdread);

            recVal = select(Socket + 1, &fdread, NULL, NULL, &tv);

            bConnectionAlive = (recVal == -1) ? false : true;
        }

        if (recVal > 0) // No Timeout
        {
            memset(buffer, '\0', BuffSize);

            bytesReceived = recv(Socket, buffer, BuffSize - 1, NULL);

            if (bytesReceived == 0)
            {
                bConnectionAlive = true;
            }
            else if (bytesReceived > 0)
            {
                strRet += buffer;
                bConnectionAlive = true;
            }
            else
            {
                bytesReceived = 0;
                bConnectionAlive = (WSAGetLastError() == WSAEWOULDBLOCK) ? true : false;
            }
        }
    } while (bConnectionAlive == true && bytesReceived > 0 && bytesReceived == (BuffSize - 1));

    return (bConnectionAlive);
}

/// <summary>
/// Thread terminated. It is safe to clean up all resources allocated for it
/// </summary>
/// <param name="Socket"></param>
void CSocketServer::ThreadTerminated(int Socket)
{
    m_Mutex.lock();

    for (std::map<int, CThreadParams*>::iterator it = m_Connections.begin(); it != m_Connections.end(); ++it)
    {
        if (it->first == Socket)
        {
            CThreadParams* pTParams = it->second;
            assert(Socket == pTParams->Socket);
            closesocket(pTParams->Socket);
            m_Connections.erase(it);
            break;
        }
    }

    m_Mutex.unlock();
}