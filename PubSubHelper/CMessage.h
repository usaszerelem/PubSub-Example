#pragma once

using namespace std;

// Need to store namespaces and information about who is subscrbed to receive events when

class CMessage
{
public:
    enum Key
    {
        Type,       // Publish/Subscribe
        IpAddress,
        Port,
        Namepace,
        AppName
    };

public:
    CMessage(bool IsPublish);       // True=Publish, False=Subscribe
    CMessage(std::string strJson);
    ~CMessage();

    CMessage operator=(const CMessage src);

    bool IsPublish();
    void AddData(Key key, std::string strValue);
    void AddData(const char *pszKey, std::string strValue);
    string FindData(Key key);
    string ToJson();

private:
    void ParseJson(string strJson);
    static size_t ParseOneJsonLine(size_t pos, std::string strJson, std::string& strKey, std::string& strVal);
    static std::string StringFormat(std::string strKey, std::string strValue);
    static const char* KeyToString(Key key);

    std::map<string, string> m_DataMap;
};
