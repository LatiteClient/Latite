#pragma once
#include "../Command.h"

class SignCommand : public Command
{
public:
	SignCommand();

	bool execute(std::string const label, std::vector<std::string> args) override;
};

