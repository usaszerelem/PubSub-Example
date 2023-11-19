#pragma once

class CPublishParam
{
public:
    inline CPublishParam()
        : m_AppName(), m_Namespace(), m_PubSubServerIp(), m_PubSubServerPort(0)
    {
    }

    inline CPublishParam operator =(const CPublishParam src)
    {
        m_AppName = src.m_AppName;
        m_Namespace = src.m_Namespace;
        m_PubSubServerIp = src.m_PubSubServerIp;
        m_PubSubServerPort = src.m_PubSubServerPort;

        return *this;
    }

    inline bool HasAllParams()
    {
        bool bOk = false;

        if (m_AppName.length() > 0 &&
            m_Namespace.length() > 0 &&
            m_PubSubServerIp.length() > 0 &&
            m_PubSubServerPort > 0)
        {
            bOk = true;
        }

        return (bOk);
    }

    std::string m_AppName;
    std::string m_Namespace;
    std::string m_PubSubServerIp;
    int         m_PubSubServerPort;
};


class CPublishThread
{
public:
    CPublishThread(CPublishParam param);
    ~CPublishThread();

private:
    CPublishParam m_param;

private:
    static void SignalHandler(int signum);
    static void ThreadWorker(CPublishThread*);
    static bool m_bTerminate;
    void PublishData();
};
