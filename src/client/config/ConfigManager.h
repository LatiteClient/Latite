/* Copyright (C) Imrglop, All rights reserved
 * Copyright (C) Latite Client contributors, All rights reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited without the permission of the author.
 * Proprietary and confidential
 * Written by Imrglop <contact: latiteclientgithub@gmail.com>, 2023
 */

#pragma once
#include "api/config/ConfigManager.h"

class ConfigManager : public IConfigManager {
public:
	ConfigManager();
	~ConfigManager() = default;

	bool loadMaster();
	bool saveCurrentConfig();
	bool saveTo(std::wstring const& name);
	bool loadUserConfig(std::wstring const& name);

	std::filesystem::path getUserPath();
private:
	bool load(std::shared_ptr<Config> cfg);
	bool save(std::shared_ptr<Config> cfg);
};