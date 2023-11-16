#include "stdafx.h"
#include <CSocketClient.h>
#include <LocalTime.h>
#include <CMessage.h>
#include <CSocket.h>
#include "CPublishThread.h"

/// <summary>
/// How the message publisher should identify itself towards the
/// pubsub server
/// </summary>
/// <param name="publishName">Name to identify as</param>
/// <param name="publishNamespace">Namespace that it will be publishing into</param>
CPublishThread::CPublishThread(
    const std::string publishName, const std::string publishNamespace, bool* pbTerminate)
    : m_publishName(publishName), m_publishNamespace(publishNamespace), m_pbTerminate(pbTerminate)
{
    thread worker(&CPublishThread::ThreadWorker, this);
    worker.join();
}

CPublishThread::~CPublishThread()
{
}

/// <summary>
/// Static thread function that just calls the 'this' instance
/// specific function.
/// </summary>
/// <param name="pThis"></param>
void CPublishThread::ThreadWorker(CPublishThread* pThis)
{
    pThis->PublishData();
}

/// <summary>
/// This thread just wakes up every few seconds and send a random message
/// to the specified namespace.
/// </summary>
void CPublishThread::PublishData()
{ 
    bool bTerminate;
	CSocketClient* pSocketClient = new CSocketClient();

    const int MaxMessageCount = 100;
    int count = 0;

	cout << "Publish Client worker thread started." << endl;
	cout << "A random message will be sent every few seconds..." << endl << endl;

    do
    {
        count++;
        this_thread::sleep_for(chrono::milliseconds(3000ms));

        m_Mutex.lock();
        bTerminate = *m_pbTerminate;
        m_Mutex.unlock();

        // TODO: Server's IP/Port needs to be externally configurable.

        if (bTerminate == false && pSocketClient->ConnectTo("127.0.0.1", 8080) == true)
        {
            if (bTerminate == false)
            {
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
