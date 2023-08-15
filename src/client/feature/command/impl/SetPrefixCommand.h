#pragma once
#include "../Command.h"

class SetPrefixCommand final : public Command {
public:
	SetPrefixCommand();
	~SetPrefixCommand() = default;

	bool execute(std::string const label, std::vector<std::string> args) override;
};