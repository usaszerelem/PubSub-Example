#pragma once

class CmdLine
{
public:
    CmdLine(int argc, char* argv[]);
    ~CmdLine();

    bool Exists(const char* pszKey);
    std::string Find(const char* pszKey);
    std::string operator [](const char* pszKey);

private:
    static std::string trim(std::string& str);
    std::map<const char*, std::string>  m_CmdMap;
};
