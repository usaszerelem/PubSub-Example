#include "pch.h"
#include "CmdLine.h"

using namespace std;

/// <summary>
/// The first element of the array, argv[0], is a pointer to the character
/// array that contains the program name or invocation name of the program
/// that is being run from the command line. argv[1] indicates the first
/// argument passed to the program, argv[2] the second argument, and so on.
/// </summary>
/// <param name="argc">Is an integer that indicates how many arguments were
///  entered on the command line when the program was started.</param>
/// <param name="argv">Is an array of pointers to arrays of character objects.</param>
CmdLine::CmdLine(int argc, char* argv[])
    : m_CmdMap()
{
    string strCmdLine;
    
    for (int i = 1; i < argc; i++)
    {
        string str = argv[i];
        size_t pos = str.find('-', 0);

        if (pos == string::npos || pos != 0)
        {
            throw std::runtime_error("Invalid command line arguments");
        }

        size_t separatorPos = str.find('=', 0);

        if (separatorPos != string::npos)
        {
            string strKey = str.substr(pos + 1, separatorPos - 1);
            strKey = trim(strKey);
            assert(strKey.length() > 0);

            string strVal = str.substr(separatorPos + 1);
            strVal = trim(strVal);
            assert(strVal.length() > 0);
            m_CmdMap[strKey] = strVal;
        }
        else
        {
            string strKey = str.substr(pos + 1);
            string strVal;
            m_CmdMap[strKey] = strVal;
        }
    }
}

/// <summary>
/// Trim 
/// </summary>
/// <param name="str"></param>
/// <returns></returns>
string CmdLine::trim(string& str)
{
    str.erase(str.find_last_not_of(' ') + 1);   //suffixing spaces
    str.erase(0, str.find_first_not_of(' '));   //prefixing spaces
    return str;
}

CmdLine::~CmdLine()
{
    m_CmdMap.clear();
}

bool CmdLine::Exists(const char* pszKey)
{
    return (m_CmdMap.find(pszKey) == m_CmdMap.end() ? false : true);
}

string CmdLine::Find(const char* pszKey)
{
    string strRet;

    if (Exists(pszKey) == true)
    {
        strRet = m_CmdMap[pszKey];
    }

    return strRet;
}

string CmdLine::operator [](const char* pszKey)
{
    return Find(pszKey);
}