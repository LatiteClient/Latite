#pragma once
#include <map>

class English
{
public:
    English();
    std::wstring getString(std::string id);
private:
    std::map<std::string, std::wstring> strings;
};

