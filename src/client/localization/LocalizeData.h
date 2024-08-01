#pragma once

class LocalizeData {
public:
	struct Language {
		std::string name;

		// int: An embedded resource
		// string: A network resource
		std::variant<int, std::string> resource;
	};

	LocalizeData();

	std::string getResourceContent(const std::variant<int, std::string>& resource);
    bool parseLangFile(Language& lang, const std::string& content, bool updateCache);
	void loadLanguage(Language& lang, bool updateCache);

	const std::vector<Language>& getLanguages() const {
		return languages;
	}

	std::wstring get(const std::string& key) {
		return localizeCache.at(key);
	}
public:
	std::vector<Language> languages;
	std::unordered_map<std::string, std::wstring> localizeCache;
};