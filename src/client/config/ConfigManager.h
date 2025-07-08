#pragma once
#include "client/config/Config.h"

class ConfigManager final : public Manager<Config> {
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
    std::shared_ptr<Config> masterConfig;
    std::shared_ptr<Config> loadedConfig;

	bool load(std::shared_ptr<Config> cfg);
	bool save(std::shared_ptr<Config> cfg);
};