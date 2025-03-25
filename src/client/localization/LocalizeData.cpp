#include "pch.h"
#include "LocalizeData.h"
#include <client/Latite.h>
#include "resource.h"

LocalizeData::LocalizeData() {
    fallbackLanguage = std::make_shared<Language>(LANG_EN_US, "en");

    languages = {
        fallbackLanguage,
#ifdef LATITE_DEBUG
        std::make_shared<Language>(LANG_AR_AR, "ar"),
#endif
        std::make_shared<Language>(LANG_CS_CZ, "cs"),
        std::make_shared<Language>(LANG_NL_NL, "nl"),
        std::make_shared<Language>(LANG_FR_FR, "fr"),
        std::make_shared<Language>(LANG_ES_ES, "es"),
        std::make_shared<Language>(LANG_JA_JP, "ja"),
        std::make_shared<Language>(LANG_PT_PT, "pt-PT"),
        std::make_shared<Language>(LANG_PT_BR, "pt-BR"),
        std::make_shared<Language>(LANG_ZH_CN, "zh-CN"),
        std::make_shared<Language>(LANG_ZH_TW, "zh-TW"),
    };

    for (auto& lang : languages) {
        loadLanguage(*lang, true);
    }
}

std::string LocalizeData::getResourceContent(const std::variant<int, std::string>& resource) {
    if (auto res = std::get_if<int>(&resource)) {
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
        for (auto it = obj["translations"].begin(); it != obj["translations"].end(); ++it) {
            lang.localizeCache[it.key()] = util::StrToWStr(it.value().get<std::string>());
        }
    }

    return true;
}

std::wstring LocalizeData::get(const std::string& id) {
    auto& lang = *languages.at(Latite::get().getSelectedLanguage());
    return tryGetKey(lang, id)
        .value_or(tryGetKey(*fallbackLanguage, id)
            .value_or(util::StrToWStr(id)));
}

std::optional<std::wstring> LocalizeData::tryGetKey(Language& lang, const std::string& key) {
    auto& cache = lang.localizeCache;
    auto it = cache.find(key);

    if (it != cache.end()) {
        return it->second;
    }

    return std::nullopt;
}
