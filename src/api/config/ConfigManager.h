#pragma once
#include "api/manager/Manager.h"
#include "Config.h"

class IConfigManager : public Manager<Config> {
public:
	std::shared_ptr<Config> masterConfig;

	IConfigManager() {}
	virtual ~IConfigManager() = default;
};