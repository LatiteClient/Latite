#pragma once
#include "api/manager/FeatureManager.h"
#include "api/feature/module/Module.h"

class IModuleManager : public FeatureManager<Module> {
public:
	virtual ~IModuleManager() = default;
};