#include "pch.h"
#include "CMessage.h"
#include "CSocket.h"

/// <summary>
/// Internal use read only static variables to identify certain key
/// fields within messages
/// </summary>
static const char* pszAppName	= "appname";
static const char* pszNamespace = "namespace";
static const char* pszIpAddress = "ip";
static const char* pszPort		= "port";
static const char* pszObjType	= "objtype";

static const char* pszPublish	= "Publish";	// Publish to message on a namespace
static const char* pszSubscribe = "Subscribe";	// Subscribe to messages on a namespace

/// <summary>
/// Initialize either a publish or a subscribe type of message
/// </summary>
/// <param name="IsPublish">Boolean True if publish, False is Subscribe</param>
CMessage::CMessage(bool IsPublish)
	: m_DataMap()
{
	// Information to send is stored as key/value pairs internally
	m_DataMap[pszObjType] = (IsPublish == true) ? pszPublish : pszSubscribe;
}

/// <summary>
/// Message constructor that is initialized from a simple
/// flat JSON string.
/// </summary>
/// <param name="strJson">Simple flat JSON string</param>
CMessage::CMessage(string strJson)
	: m_DataMap()
{
	ParseJson(strJson);
}

CMessage::~CMessage()
{
}

/// <summary>
/// Copy operator in case there is a need to copy a message
/// </summary>
/// <param name="src">From message object</param>
/// <returns></returns>
CMessage CMessage::operator=(const CMessage src)
{
	m_DataMap = src.m_DataMap;
    return *this;
}

/// <summary>
/// Based on the message object type field returns a Boolean value
/// indicating whether this message Publish or Subscribe.
/// Internal error is thrown is field is missing from the message,
/// which should never happen.
/// </summary>
/// <returns>Boolean True if Publish, False is Subscribe</returns>
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

/// <summary>
/// Adds a key/value pair field to the message. Intentionally
/// not guarding for duplicates as I trust the implementer
/// knows what needs to be done.
/// </summary>
/// <param name="key">Unique key value identifying the value</param>
/// <param name="strValue">Value</param>
void CMessage::AddData(Key key, std::string strValue)
{
	m_DataMap[KeyToString(key)] = strValue;
}

/// <summary>
/// Adds a key/value pair field to the message. Intentionally
/// not guarding for duplicates as I trust the implementer
/// knows what needs to be done.
/// </summary>
/// <param name="key">Unique key value identifying the value</param>
/// <param name="strValue">Value</param>
void CMessage::AddData(const char* pszKey, std::string strValue)
{
	m_DataMap[pszKey] = strValue;
}

/// <summary>
/// Finds a specified field value that is identified with the passed
/// in unique key identifier.
/// </summary>
/// <param name="key">Unique identifier to find</param>
/// <returns>Value associated with the field, or empty string if not found</returns>
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

/// <summary>
/// Very simple JSON parser for the flat data formats that we
/// are using. Similar to a SAX parser, it parses one line at a
/// time and returns one parsed JSON name/value pair.
/// </summary>
/// <param name="pos">Data position marker</param>
/// <param name="strJson">JSON formatted string to parse</param>
/// <param name="strKey">reference to a string that holds the key</param>
/// <param name="strVal">reference to a string that holds the value</param>
/// <returns>Updated position marker</returns>
size_t CMessage::ParseOneJsonLine(size_t pos, string strJson, string& strKey, string& strVal)
{
	int parseStep = 0;
	bool bDone = false;

	do
	{
		switch (parseStep++)
		{
			case 0:
				// "appname": "AbcApp",
				// ^
				pos = strJson.find_first_of('\"', pos);

				if (pos == string::npos)
				{
					pos = strJson.find_first_of('}', pos);
					bDone = true;
				}
				break;

			case 1:
				{
					// "appname": "AbcApp",
					//  ^^^^^^^
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
				// "appname": "AbcApp",
				//            ^
				pos = strJson.find_first_of('\"', pos);
				break;

			case 3:
				{
				// "appname": "AbcApp",
				//             ^^^^^^
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

/// <summary>
/// Converts internal message format to JSON format. 
/// </summary>
/// <returns>JSON string</returns>
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

/// <summary>
/// Internal function that takes a key/value pair and
/// returns a JSON line
/// </summary>
/// <param name="strKey">Unique key for the field</param>
/// <param name="strValue">Value associated with the key</param>
/// <returns>JSON string</returns>
string CMessage::StringFormat(string strKey, string strValue)
{
	string strRet = "\"";
	strRet += strKey;
	strRet += "\": \"";
	strRet += strValue;
	strRet += "\",";

	return (strRet);
}

/// <summary>
/// Maps an enum ID to its unique string identifier so that
/// this field can be looked up in the map data structure.
/// </summary>
/// <param name="key">Enum key identifier</param>
/// <returns>pointer to a string</returns>
const char* CMessage::KeyToString(Key key)
{
	const static struct
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