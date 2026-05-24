#pragma once
#include <string>
#include <utility>

class LocalizedString {
public:
    LocalizedString(std::string key, std::wstring value)
        : localizationKey(std::move(key)), localizedValue(std::move(value)) {}

    [[nodiscard]] const std::string& key() const {
        return localizationKey;
    }

    [[nodiscard]] const std::wstring& value() const {
        return localizedValue;
    }

    operator std::wstring() const {
        return localizedValue;
    }

private:
    std::string localizationKey;
    std::wstring localizedValue;
};

class LocalizeString
{
public:
    static LocalizedString get(const std::string& id);
};

