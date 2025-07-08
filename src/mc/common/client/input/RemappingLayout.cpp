#include "pch.h"
#include "RemappingLayout.h"
#include "PCH.h"
#include <optional>

int SDK::RemappingLayout::findValue(std::string const& name) {
    for (size_t i = 0; i < this->inputSettings.size(); i++) {
        auto set = inputSettings[i];
        if ("key." + name == set.name) {
            return set.getFirstKey();
        }
    }
    return 0;
}

int SDK::RemappingLayout::findValueFromOriginal(int val)
{
    std::optional<size_t> idx;
    for (size_t i = 0; i < this->defaultSettings.size(); i++) {
        if (this->defaultSettings[i].getFirstKey() == val) {
            idx = i;
            break;
        }
    }
    if (idx.has_value()) {
        return this->inputSettings[idx.value()].getFirstKey();
    }
    return val;
}
