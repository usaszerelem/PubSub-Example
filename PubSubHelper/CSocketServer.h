#pragma once

class CSocketServerDataReceived
{
public:
    virtual void DataReceived(std::string) = 0;
    virtual bool Terminate() = 0;
};

class CSocketServer
{
public:
    CSocketServer(CSocketServerDataReceived * pCallback, int ListenPort);
    ~CSocketServer();

    void AcceptConnections();

private:

    class CThreadParams
    {
        public:
            inline CThreadParams()
                : pCallback(nullptr), Socket(-1), bTerminate(false), pSocketServer(nullptr)
            {
            }

            inline ~CThreadParams()
            {
            }

        public:
            CSocketServerDataReceived* pCallback;
            int             Socket;
            std::thread     worker;
            bool            bTerminate;
            CSocketServer* pSocketServer;
    };


    static void ThreadWorker(CThreadParams * pTData);
    static bool ReadReceivedData(int Socket, std::string & strRet);
    void Initialize(int ListenPort);
    void ThreadTerminated(int Socket);

private:
    int                         m_Socket;
    std::mutex                  m_Mutex;
    std::map<int, CThreadParams*> m_Connections;
    CSocketServerDataReceived * m_pCallback;
};
