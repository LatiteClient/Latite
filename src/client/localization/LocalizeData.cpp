#include "pch.h"
#include "LocalizeData.h"
#include <client/Latite.h>
#include "resource.h"

LocalizeData::LocalizeData() {
    // Add English as a language by default.
    auto defaultLanguage = Language{
        .resource = LANG_EN_US };
    loadLanguage(defaultLanguage, true);
    languages.push_back(defaultLanguage);
}

std::string LocalizeData::getResourceContent(const std::variant<int, std::string>& resource) {
    if (auto res = std::get_if<int>(&resource)) {

        // TODO: Doesn't account for unicode
        return Latite::get().getTextAsset(*res);
    }

    throw std::runtime_error("Network resource is not implemented");
}

void LocalizeData::loadLanguage(Language& lang, bool cache) {
    std::string content = getResourceContent(lang.resource);
    parseLangFile(lang, content, cache);
}

bool LocalizeData::parseLangFile(Language& lang, const std::string& content, bool cache) {

    auto obj = json::parse(content);

    if (!obj.is_object()) return false;
    if (!obj["name"].is_string()) return false;
    if (!obj["translations"].is_object()) return false;

    lang.name = obj["name"].get<std::string>();

    if (cache) {
        for (auto it = obj["translations"].begin(); it != obj.end(); ++it) {
            localizeCache[it.key()] = util::StrToWStr(it.value().get<std::string>());
        }
    }

    return true;
}
