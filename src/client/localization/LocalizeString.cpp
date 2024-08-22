#include "pch.h"
#include "LocalizeString.h"

std::wstring LocalizeString::get(const std::string& id) {
    return Latite::get().getL10nData().get(id);
}
