#pragma once
#include "api/manager/FeatureManager.h"
#include "Module.h"

class ModuleManager : public FeatureManager<Module> {
public:
	~ModuleManager() = default;
};