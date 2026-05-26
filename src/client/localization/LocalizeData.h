#pragma once
#include <utility>

#include "client/resource/Resource.h"

class LocalizeData {
public:
	static constexpr int systemDefaultLanguageSettingValue = 0;

	struct Language {
		std::string name;
		std::string langCode;
		bool rightToLeft = false;
		Resource resource;

		std::unordered_map<std::string, std::wstring> localizeCache;

		explicit Language(const Resource resource, std::string langCode)
			: langCode(std::move(langCode)), resource(resource) {}
	};

	LocalizeData();

	bool parseLangFile(Language& lang, const std::string& content, bool updateCache);
	void loadLanguage(Language& lang, bool updateCache);

	[[nodiscard]] const std::vector<std::shared_ptr<Language>>& getLanguages() const {
		return languages;
	}

	[[nodiscard]] bool isSelectedLanguageRightToLeft() const;
	[[nodiscard]] int resolveLanguageSetting(int selectedLanguageSetting) const;
	std::wstring get(const std::string& key);
private:
	[[nodiscard]] int getSystemDefaultLanguageIndex() const;
	std::optional<std::wstring> tryGetKey(Language& lang, const std::string& key);
	std::shared_ptr<Language> fallbackLanguage;
	std::vector<std::shared_ptr<Language>> languages;
};
