#pragma once
#include "api/feature/module/ModuleManager.h"

class ModuleManager : public IModuleManager {
public:
	void init() override;
};