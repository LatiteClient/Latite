#include "pch.h"
#include "English.h"

English::English() {
    English::strings = {
        { L"intro.welcome", L"Welcome to Latite Client!" },
        { L"intro.menubutton", L"Press " + util::StrToWStr(util::KeyToString(Latite::get().getMenuKey().value)) + L" in a world or server to open the mod menu." }
    };
}

std::wstring English::getString(std::wstring id) {
    return English::strings.at(id);
}