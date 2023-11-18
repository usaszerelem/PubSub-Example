#pragma once

class CPublishParam
{
public:
    inline CPublishParam()
        : m_publishName(), m_publishNamespace(), m_publishUrl(), m_publishPort(0)
    {
    }

    inline CPublishParam operator =(const CPublishParam src)
    {
        m_publishName = src.m_publishName;
        m_publishNamespace = src.m_publishNamespace;
        m_publishUrl = src.m_publishUrl;
        m_publishPort = src.m_publishPort;

        return *this;
    }

    inline bool HasAllParams()
    {
        bool bOk = false;

        if (m_publishName.length() > 0 &&
            m_publishNamespace.length() > 0 &&
            m_publishUrl.length() > 0 &&
            m_publishPort > 0)
        {
            bOk = true;
        }

        return (bOk);
    }

    std::string m_publishName;
    std::string m_publishNamespace;
    std::string m_publishUrl;
    int         m_publishPort;
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
