#include "stdafx.h"
#include <CSocketClient.h>
#include <LocalTime.h>
#include <CMessage.h>
#include <CSocket.h>
#include "CPublishThread.h"

CPublishThread::CPublishThread(const std::string publishName, const std::string publishNamespace)
{
    m_publishName = publishName;
	m_publishNamespace = publishNamespace;
}

CPublishThread::~CPublishThread()
{
}

bool CPublishThread::Init(bool * pbTerminate)
{
    m_pbTerminate = pbTerminate;
    thread worker(&CPublishThread::ThreadWorker, this);

    worker.join();
    return false;
}
void CPublishThread::ThreadWorker(CPublishThread* pThis)
{
    pThis->PublishData();
}

void CPublishThread::PublishData()
{ 
    bool bTerminate;
	CSocketClient* pSocketClient = new CSocketClient();

    const int MaxMessageCount = 100;
    int count = 0;

	cout << "Publish Client worker thread started." << endl;
	cout << "An event will be published every few seconds..." << endl << endl;

    do
    {
        count++;
        this_thread::sleep_for(chrono::milliseconds(3000ms));

        m_Mutex.lock();
        bTerminate = *m_pbTerminate;
        m_Mutex.unlock();

        if (bTerminate == false && pSocketClient->ConnectTo("127.0.0.1", 8080) == true)
        {
            if (bTerminate == false)
            {
                //CMessage PublishData(m_publishName, m_publishNamespace, CMessage::DataType::Publish);

                CMessage PublishData(true);
                PublishData.AddData(CMessage::Key::AppName, m_publishName);
                PublishData.AddData(CMessage::Key::Namepace, m_publishNamespace);
                PublishData.AddData(CMessage::Key::IpAddress, GetLocalIp());
                PublishData.AddData("time", GetLocalTime());

                string dataToSend = PublishData.ToJson();

                cout << count << ": Sending: " << dataToSend << endl;
                pSocketClient->Send(dataToSend);
            }
        }
    } while (bTerminate == false && count < MaxMessageCount);

	delete pSocketClient;

    cout << "Publish Client worker thread terminated.";
}
