#include "pch.h"
#include "CMessage.h"
#include "CSocket.h"

static const char* pszAppName	= "appname";
static const char* pszNamespace = "namespace";
static const char* pszIpAddress = "ip";
static const char* pszPort		= "port";
static const char* pszObjType	= "objtype";

static const char* pszPublish	= "Publish";
static const char* pszSubscribe = "Subscribe";

CMessage::CMessage(bool IsPublish)
	: m_DataMap()
{
	m_DataMap[pszObjType] = (IsPublish == true) ? pszPublish : pszSubscribe;
}

CMessage::CMessage(string strJson)
	: m_DataMap()
{
	ParseJson(strJson);
}

CMessage::~CMessage()
{
}

CMessage CMessage::operator=(const CMessage src)
{
	m_DataMap = src.m_DataMap;
    return *this;
}

bool CMessage::IsPublish()
{
	string str = FindData(Type);
	assert(str.length() > 0);

	if (str == pszPublish)
		return true;

	if (str == pszSubscribe)
		return false;

	throw std::runtime_error("Internal Error - IsPublish()");
}

void CMessage::AddData(Key key, std::string strValue)
{
	m_DataMap[KeyToString(key)] = strValue;
}

void CMessage::AddData(const char* pszKey, std::string strValue)
{
	m_DataMap[pszKey] = strValue;
}

string CMessage::FindData(Key key)
{
	string strRet;
	std::map<string, string>::iterator it = m_DataMap.find(KeyToString(key));

	if (it != m_DataMap.end())
	{
		strRet = it->second;
	}

	return (strRet);
}

/*
* Very simple flat JSON parser just for proof of concept
* {
*	 "appname": "AbcApp",
*	 "namespace": "whatever",
*	 "ip": "127.0.0.1",
*    "port": "8080",
*	 "datatype": "Publish",
* }
*/
void CMessage::ParseJson(string strJson)
{
	size_t pos = 0;

	pos = strJson.find('{', pos);

	if (pos == string::npos) {
		throw std::runtime_error("JSON parser cannot parse: " + strJson);
	}

	do
	{
		string strKey;
		string strVal;
		pos = ParseOneJsonLine(pos, strJson, strKey, strVal);

		if (pos != string::npos)
		{
			assert(strKey.length() > 0);
			assert(strVal.length() > 0);

			m_DataMap[strKey.c_str()] = strVal;
		}
	} while (pos != string::npos);
}

size_t CMessage::ParseOneJsonLine(size_t pos, string strJson, string& strKey, string& strVal)
{
	int parseStep = 0;
	bool bDone = false;

	do
	{
		switch (parseStep++)
		{
			case 0:
				pos = strJson.find_first_of('\"', pos);

				if (pos == string::npos)
				{
					pos = strJson.find_first_of('}', pos);
					bDone = true;
				}
				break;

			case 1:
				{
					size_t endPos = strJson.find_first_of('\"', pos + 1);

					if (endPos != string::npos)
					{
						strKey = strJson.substr(pos + 1, endPos - pos - 1);
						endPos++;
					}

					pos = endPos;
				}
				break;

			case 2:
				pos = strJson.find_first_of('\"', pos);
				break;

			case 3:
				{
					size_t endPos = strJson.find_first_of('\"', pos + 1);

					if (endPos != string::npos)
					{
						strVal = strJson.substr(pos + 1, endPos - pos - 1);
						endPos++;
					}

					pos = endPos;
				}
				break;

			default:
				bDone = true;
				break;
		}

		if (pos == string::npos && bDone == false) {
			throw std::runtime_error("JSON parser cannot parse: " + strJson);
		}
	} while (bDone == false);

	return(pos);
}


string CMessage::ToJson()
{
	string strRet;

	try
	{
		strRet = "{";

		for (auto it = m_DataMap.begin(); it != m_DataMap.end(); it++)
		{
			strRet += StringFormat(it->first, it->second);
		}

		strRet += "}";
	}
	catch (...)
	{
		cout << "Internal error in CreatePublishJson()";
	}

	return strRet;
}

string CMessage::StringFormat(string strKey, string strValue)
{
	string strRet = "\"";
	strRet += strKey;
	strRet += "\": \"";
	strRet += strValue;
	strRet += "\",";

	return (strRet);
}

const char* CMessage::KeyToString(Key key)
{
	static struct
	{
		CMessage::Key key;
		const char* pszKey;
	} KeyMap [] =
	{
		{ CMessage::Key::Type, pszObjType },
		{ CMessage::Key::IpAddress, pszIpAddress },
		{ CMessage::Key::Port, pszPort },
		{ CMessage::Key::Namepace, pszNamespace },
		{ CMessage::Key::AppName, pszAppName },
	};

	const char* pszRet = nullptr;

	for (int idx = 0; idx < sizeof(KeyMap) / sizeof(KeyMap[0]); idx++)
	{
		if (KeyMap[idx].key == key)
		{
			pszRet = KeyMap[idx].pszKey;
			break;
		}
	}

	assert(pszRet != nullptr);

	return pszRet;
}