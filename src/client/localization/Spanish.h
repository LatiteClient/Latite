#pragma once
#include <map>

class Spanish
{
public:
    Spanish();
    std::wstring getString(std::string id);
private:
    std::map<std::string, std::wstring> strings;
};

