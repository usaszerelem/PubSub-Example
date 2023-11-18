#include "stdafx.h"
#include <CSocketClient.h>
#include <LocalTime.h>
#include <CMessage.h>
#include <CSocket.h>
#include "CPublishThread.h"

using namespace std;

bool CPublishThread::m_bTerminate = false;

/// <summary>
/// How the message publisher should identify itself towards the
/// pubsub server
/// </summary>
/// <param name="publishName">Name to identify as</param>
/// <param name="publishNamespace">Namespace that it will be publishing into</param>
CPublishThread::CPublishThread(CPublishParam params)
    : m_param(params)
{
    assert(m_param.HasAllParams() == true);
    signal(SIGINT, SignalHandler);

    thread worker(&CPublishThread::ThreadWorker, this);
    worker.join();
}

CPublishThread::~CPublishThread()
{
}

// ---------------------------------------------------------------------------
// Define the function to be called when ctrl-c (SIGINT) is sent to process
// ---------------------------------------------------------------------------

void CPublishThread::SignalHandler(int signum) {
    cout << "Caught signal " << signum << endl;

    m_bTerminate = true;
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
	CSocketClient* pSocketClient = new CSocketClient();

    const int MaxMessageCount = 100;
    int count = 0;

	cout << "Publish Client worker thread started." << endl;
	cout << "A random message will be sent every few seconds..." << endl << endl;

    do
    {
        count++;
        this_thread::sleep_for(chrono::milliseconds(3000ms));

        if (m_bTerminate == false && pSocketClient->ConnectTo(m_param.m_publishUrl, m_param.m_publishPort) == true)
        {
            if (m_bTerminate == false)
            {
                CMessage PublishData(true);
                PublishData.AddData(CMessage::Key::AppName, m_param.m_publishName);
                PublishData.AddData(CMessage::Key::Namepace, m_param.m_publishNamespace);
                PublishData.AddData(CMessage::Key::IpAddress, GetLocalIp());
                PublishData.AddData("time", GetLocalTime());

                string dataToSend = PublishData.ToJson();

                cout << count << ": Sending: " << dataToSend << endl;
                pSocketClient->Send(dataToSend);
            }
        }
    } while (m_bTerminate == false && count < MaxMessageCount);

	delete pSocketClient;

    cout << "Publish Client worker thread terminated.";
}
