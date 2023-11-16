#pragma once

class CPublishThread
{
public:
    CPublishThread(const std::string publishName, const std::string publishNamespace, bool* pbTerminate);
    ~CPublishThread();

private:
    std::string  m_publishName;
    std::string  m_publishNamespace;
    std::string  m_localIp;
    std::mutex   m_Mutex;
    bool  *      m_pbTerminate;

private:
    static void ThreadWorker(CPublishThread*);
    void PublishData();
};
