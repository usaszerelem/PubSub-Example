#include "pch.h"
#include "CSocket.h"

using namespace std;

/// <summary>
/// Returns the most likely IP address of the local computer.
/// This is not 100% as there can be several NIC cards.
/// </summary>
/// <returns>Local computer's IP address</returns>
string GetLocalIp()
{
    char szBuffer[1024] = { 0 };

    if (gethostname(szBuffer, sizeof(szBuffer)) != SOCKET_ERROR)
    {
        struct hostent* host = gethostbyname(szBuffer);

        //Obtain the computer's IP
        unsigned int num1 = (int)((struct in_addr*)(host->h_addr))->S_un.S_un_b.s_b1;
        unsigned int num2 = (int)((struct in_addr*)(host->h_addr))->S_un.S_un_b.s_b2;
        unsigned int num3 = (int)((struct in_addr*)(host->h_addr))->S_un.S_un_b.s_b3;
        unsigned int num4 = (int)((struct in_addr*)(host->h_addr))->S_un.S_un_b.s_b4;

        sprintf_s(szBuffer, "%u.%u.%u.%u", num1, num2, num3, num4);
    }

    return (szBuffer);
}
