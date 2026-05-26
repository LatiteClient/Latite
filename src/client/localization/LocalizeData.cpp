#include "pch.h"
#include <algorithm>
#include <cctype>
#include "LocalizeData.h"
#include <client/Latite.h>
#include "client/resource/Resource.h"
#include "client/resource/InitResources.h"

namespace {
    constexpr int fallbackLanguageIndex = 0;

    std::string normalizeLocaleTag(std::string_view value) {
        std::string normalized;
        normalized.reserve(value.size());

        for (char ch : value) {
            normalized.push_back(ch == '_'
                ? '-'
                : static_cast<char>(std::tolower(static_cast<unsigned char>(ch))));
        }

        return normalized;
    }

    bool isAsciiAlpha(std::string_view value) {
        return std::all_of(value.begin(), value.end(), [](char ch) {
            return std::isalpha(static_cast<unsigned char>(ch)) != 0;
        });
    }

    std::string localeLanguage(std::string_view locale) {
        const std::string normalized = normalizeLocaleTag(locale);
        const auto split = normalized.find('-');
        return split == std::string::npos ? normalized : normalized.substr(0, split);
    }

    std::optional<std::string> localeRegion(std::string_view locale) {
        const std::string normalized = normalizeLocaleTag(locale);
        std::string_view view = normalized;

        for (std::size_t start = 0, subtagIndex = 0; start < view.size(); ++subtagIndex) {
            const auto end = view.find('-', start);
            const auto subtag = view.substr(start, end == std::string_view::npos ? std::string_view::npos : end - start);

            if (subtagIndex > 0 && subtag.size() == 2 && isAsciiAlpha(subtag)) {
                return std::string(subtag);
            }

            if (end == std::string_view::npos) break;
            start = end + 1;
        }

        return std::nullopt;
    }

    bool localeHasSubtag(std::string_view locale, std::string_view expectedSubtag) {
        const std::string normalized = normalizeLocaleTag(locale);
        const std::string expected = normalizeLocaleTag(expectedSubtag);
        std::string_view view = normalized;

        for (std::size_t start = 0; start < view.size();) {
            const auto end = view.find('-', start);
            const auto subtag = view.substr(start, end == std::string_view::npos ? std::string_view::npos : end - start);
            if (subtag == expected) return true;
            if (end == std::string_view::npos) break;
            start = end + 1;
        }

        return false;
    }

    std::optional<std::string> getUserDefaultLocaleName() {
        WCHAR localeName[LOCALE_NAME_MAX_LENGTH]{};
        if (GetUserDefaultLocaleName(localeName, LOCALE_NAME_MAX_LENGTH) <= 0) {
            return std::nullopt;
        }

        return normalizeLocaleTag(util::WStrToStr(localeName));
    }

    std::optional<std::string> getUserDefaultGeoName() {
        WCHAR geoName[16]{};
        if (GetUserDefaultGeoName(geoName, static_cast<int>(_countof(geoName))) <= 0) {
            return std::nullopt;
        }

        return normalizeLocaleTag(util::WStrToStr(geoName));
    }

    std::optional<int> findLanguageByExactLocale(const std::vector<std::shared_ptr<LocalizeData::Language>>& languages, std::string_view locale) {
        const std::string normalized = normalizeLocaleTag(locale);
        if (normalized.empty()) return std::nullopt;

        for (int i = 0; i < static_cast<int>(languages.size()); ++i) {
            if (normalizeLocaleTag(languages[i]->langCode) == normalized) {
                return i;
            }
        }

        return std::nullopt;
    }

    std::optional<int> findLanguageByRegion(const std::vector<std::shared_ptr<LocalizeData::Language>>& languages, std::string_view region) {
        const std::string normalizedRegion = normalizeLocaleTag(region);
        if (normalizedRegion.empty()) return std::nullopt;

        for (int i = 0; i < static_cast<int>(languages.size()); ++i) {
            const auto supportedRegion = localeRegion(languages[i]->langCode);
            if (supportedRegion && *supportedRegion == normalizedRegion) {
                return i;
            }
        }

        return std::nullopt;
    }

    std::optional<int> findLanguageByScript(const std::vector<std::shared_ptr<LocalizeData::Language>>& languages, std::string_view locale) {
        if (localeLanguage(locale) != "zh") return std::nullopt;

        if (localeHasSubtag(locale, "hant")) {
            return findLanguageByExactLocale(languages, "zh-TW");
        }

        if (localeHasSubtag(locale, "hans")) {
            return findLanguageByExactLocale(languages, "zh-CN");
        }

        return std::nullopt;
    }

    std::optional<int> findLanguageByBaseLocale(const std::vector<std::shared_ptr<LocalizeData::Language>>& languages, std::string_view locale) {
        const std::string language = localeLanguage(locale);
        if (language.empty()) return std::nullopt;

        for (int i = 0; i < static_cast<int>(languages.size()); ++i) {
            if (localeLanguage(languages[i]->langCode) == language) {
                return i;
            }
        }

        return std::nullopt;
    }
}

LocalizeData::LocalizeData() {
    // Lang file resources defined in client/resource/InitResources.h

    fallbackLanguage = std::make_shared<Language>(GET_RESOURCE(lang_en_US_json), "en-US");

    languages = {
        fallbackLanguage,
        std::make_shared<Language>(GET_RESOURCE(lang_cs_CZ_json), "cs-CZ"),
        std::make_shared<Language>(GET_RESOURCE(lang_nl_NL_json), "nl-NL"),
        std::make_shared<Language>(GET_RESOURCE(lang_fr_FR_json), "fr-FR"),
        std::make_shared<Language>(GET_RESOURCE(lang_es_ES_json), "es-ES"),
        std::make_shared<Language>(GET_RESOURCE(lang_ja_JP_json), "ja-JP"),
        std::make_shared<Language>(GET_RESOURCE(lang_ru_RU_json), "ru-RU"),
        std::make_shared<Language>(GET_RESOURCE(lang_pt_PT_json), "pt-PT"),
        std::make_shared<Language>(GET_RESOURCE(lang_pt_BR_json), "pt-BR"),
        std::make_shared<Language>(GET_RESOURCE(lang_zh_CN_json), "zh-CN"),
        std::make_shared<Language>(GET_RESOURCE(lang_zh_TW_json), "zh-TW"),
        std::make_shared<Language>(GET_RESOURCE(lang_ar_SA_json), "ar-SA"),
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
    lang.rightToLeft = obj.value("rtl", false);

    if (cache) {
        for (auto it = obj["translations"].begin(); it != obj["translations"].end(); ++it) {
            lang.localizeCache[it.key()] = util::StrToWStr(it.value().get<std::string>());
        }
    }

    return true;
}

int LocalizeData::resolveLanguageSetting(int selectedLanguageSetting) const {
    if (languages.empty()) return fallbackLanguageIndex;

    if (selectedLanguageSetting == LocalizeData::systemDefaultLanguageSettingValue) {
        return getSystemDefaultLanguageIndex();
    }

    const int selectedLanguage = selectedLanguageSetting - 1;
    if (selectedLanguage < 0 || selectedLanguage >= static_cast<int>(languages.size())) {
        return fallbackLanguageIndex;
    }

    return selectedLanguage;
}

int LocalizeData::getSystemDefaultLanguageIndex() const {
    if (languages.empty()) return fallbackLanguageIndex;

    std::optional<int> fallbackMatch;
    auto preferNonFallback = [&](std::optional<int> match) {
        if (!match) return std::optional<int>{};
        if (*match == fallbackLanguageIndex) {
            fallbackMatch = match;
            return std::optional<int>{};
        }
        return match;
    };

    if (const auto region = getUserDefaultGeoName()) {
        if (const auto match = preferNonFallback(findLanguageByRegion(languages, *region))) {
            return *match;
        }
    }

    if (const auto locale = getUserDefaultLocaleName()) {
        if (const auto match = preferNonFallback(findLanguageByExactLocale(languages, *locale))) {
            return *match;
        }

        if (const auto match = preferNonFallback(findLanguageByScript(languages, *locale))) {
            return *match;
        }

        if (const auto match = preferNonFallback(findLanguageByBaseLocale(languages, *locale))) {
            return *match;
        }
    }

    return fallbackMatch.value_or(fallbackLanguageIndex);
}

bool LocalizeData::isSelectedLanguageRightToLeft() const {
    const int selectedLanguage = Latite::get().getSelectedLanguage();
    if (selectedLanguage < 0 || selectedLanguage >= static_cast<int>(languages.size())) {
        return false;
    }

    return languages[selectedLanguage]->rightToLeft;
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
