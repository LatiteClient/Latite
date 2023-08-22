#pragma once
#include "../Command.h"

class ScriptCommand : public Command
{
public:
	ScriptCommand();

	bool execute(std::string const label, std::vector<std::string> args) override;
};

