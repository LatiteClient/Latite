#pragma once
#include "api/config/ConfigManager.h"

class ConfigManager : public IConfigManager {
public:
	ConfigManager();
	~ConfigManager() = default;

	bool loadMaster();

	void applyLanguageConfig(std::string_view languageSettingName);
	void applyGlobalConfig();
	void applyModuleConfig();

	bool saveCurrentConfig();
	bool saveTo(std::wstring const& name);
	bool loadUserConfig(std::wstring const& name);

	std::filesystem::path getUserPath();
private:
	bool load(std::shared_ptr<Config> cfg);
	bool save(std::shared_ptr<Config> cfg);
};