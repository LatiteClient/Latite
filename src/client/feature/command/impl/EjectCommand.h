#pragma once
#include "../Command.h"

class EjectCommand final : public Command {
public:
	EjectCommand();
	~EjectCommand() = default;

	bool execute(std::string const label, std::vector<std::string> args) override;
};