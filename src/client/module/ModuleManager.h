#pragma once
#include "api/manager/FeatureManager.h"
#include "api/feature/module/Module.h"

class ModuleManager : public FeatureManager<Module> {
public:
	~ModuleManager() = default;

	virtual void init() {};
};