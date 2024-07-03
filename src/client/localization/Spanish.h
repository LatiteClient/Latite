#pragma once
#include <map>

class Spanish
{
public:
    Spanish();
    std::wstring getString(std::wstring id);
private:
    std::map<std::wstring, std::wstring> strings;
};

