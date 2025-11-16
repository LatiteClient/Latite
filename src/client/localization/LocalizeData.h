#pragma once
#include <utility>

#include "client/resource/Resource.h"

#pragma once

class LocalizeData {
public:
	struct Language {
		std::string name;
		std::string langCode;
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

	std::wstring get(const std::string& key);
private:
	std::optional<std::wstring> tryGetKey(Language& lang, const std::string& key);
	std::shared_ptr<Language> fallbackLanguage;
	std::vector<std::shared_ptr<Language>> languages;
};