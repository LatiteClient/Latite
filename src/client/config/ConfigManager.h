#pragma once
#include "api/config/ConfigManager.h"

class ConfigManager : public IConfigManager {
public:
	ConfigManager();
	~ConfigManager() = default;

	bool loadMaster();
	bool saveCurrentConfig();
private:
	bool load(std::shared_ptr<Config> cfg);
	bool save(std::shared_ptr<Config> cfg);
};