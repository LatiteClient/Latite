#include "pch.h"
#include "LocalizeData.h"
#include <client/Latite.h>
#include "client/resource/Resource.h"
#include "client/resource/InitResources.h"

LocalizeData::LocalizeData() {
    // Lang file resources defined in client/resource/InitResources.h

    fallbackLanguage = std::make_shared<Language>(GET_RESOURCE(lang_en_US_json), "en");

    languages = {
        fallbackLanguage,
#ifdef LATITE_DEBUG
        std::make_shared<Language>(LANG_AR_AR, "ar"),
#endif
        std::make_shared<Language>(GET_RESOURCE(lang_cs_CZ_json), "cs"),
        std::make_shared<Language>(GET_RESOURCE(lang_nl_NL_json), "nl"),
        std::make_shared<Language>(GET_RESOURCE(lang_fr_FR_json), "fr"),
        std::make_shared<Language>(GET_RESOURCE(lang_es_ES_json), "es"),
        std::make_shared<Language>(GET_RESOURCE(lang_ja_JP_json), "ja"),
        std::make_shared<Language>(GET_RESOURCE(lang_pt_PT_json), "pt-PT"),
        std::make_shared<Language>(GET_RESOURCE(lang_pt_BR_json), "pt-BR"),
        std::make_shared<Language>(GET_RESOURCE(lang_zh_CN_json), "zh-CN"),
        std::make_shared<Language>(GET_RESOURCE(lang_zh_TW_json), "zh-TW"),
    };

    for (auto& lang : languages) {
        loadLanguage(*lang, true);
    }
}



void LocalizeData::loadLanguage(Language& lang, bool cache) {
    auto str = std::string(lang.resource.begin(), lang.resource.end());
    parseLangFile(lang, str, cache);
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
