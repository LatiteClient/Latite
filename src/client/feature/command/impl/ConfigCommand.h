#pragma once
#include "../Command.h"

class ConfigCommand final : public Command {
public:
	ConfigCommand();
	~ConfigCommand() = default;

	bool execute(std::string const label, std::vector<std::string> args) override;
};