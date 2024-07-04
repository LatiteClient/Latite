#pragma once
#include <map>

class English
{
public:
    English();
    std::wstring getString(std::wstring id);
private:
    std::map<std::wstring, std::wstring> strings;
};

