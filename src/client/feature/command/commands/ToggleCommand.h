#pragma once
#include "../Command.h"

class ToggleCommand final : public Command {
public:
	ToggleCommand();
	~ToggleCommand() = default;

	bool execute(std::string const label, std::vector<std::string> args) override;
};