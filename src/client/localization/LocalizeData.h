#pragma once

class LocalizeData {
public:
	struct Language {
		std::string name;

		// int: An embedded resource
		// string: A network resource
		std::variant<int, std::string> resource;
		std::unordered_map<std::string, std::wstring> localizeCache;

		Language(std::variant<int, std::string> resource) : resource(resource) {}
	};

	LocalizeData();

	std::string getResourceContent(const std::variant<int, std::string>& resource);
	bool parseLangFile(Language& lang, const std::string& content, bool updateCache);
	void loadLanguage(Language& lang, bool updateCache);

	const std::vector<std::shared_ptr<Language>>& getLanguages() const {
		return languages;
	}

	const std::vector<std::wstring>& getMicrosoftLanguages() const {
		return microsoftLanguages;
	}

	std::wstring get(const std::string& key);
private:
	std::optional<std::wstring> tryGetKey(Language& lang, const std::string& key);
	std::shared_ptr<Language> fallbackLanguage;
	std::vector<std::shared_ptr<Language>> languages;
	std::vector<std::wstring> microsoftLanguages;
};