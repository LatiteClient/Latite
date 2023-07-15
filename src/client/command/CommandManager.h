#pragma once
#include "api/manager/FeatureManager.h"
#include "Command.h"

class CommandManager final : public FeatureManager<Command> {
public:
	inline static std::string prefix = ".";

	CommandManager() = default;
	virtual ~CommandManager() = default;

	void init();
	bool runCommand(std::string const& line);
};