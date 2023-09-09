#pragma once
#include "../Command.h"

class TestCommand final : public Command {
public:
	TestCommand();
	~TestCommand() = default;

	bool execute(std::string const label, std::vector<std::string> args) override;
private:
	void onRenderLayer(Event& evG);
};