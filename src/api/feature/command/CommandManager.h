#pragma once
#include "api/manager/FeatureManager.h"
#include "api/feature/command/Command.h"

class ICommandManager : public FeatureManager<ICommand> {
public:
	virtual ~ICommandManager() = default;
};