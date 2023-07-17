#pragma once
#include "../Command.h"

class HelpCommand final : public Command {
public:
	HelpCommand();
	~HelpCommand() = default;

	bool execute(std::string const label, std::vector<std::string> args) override;
};