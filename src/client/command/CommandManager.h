#pragma once
#include "api/feature/command/CommandManager.h"
#include "Command.h"

class CommandManager final : public ICommandManager {
public:
	inline static std::string prefix = ".";

	CommandManager() = default;
	virtual ~CommandManager() = default;

	void init() override;

	bool runCommand(std::string const& line);
};