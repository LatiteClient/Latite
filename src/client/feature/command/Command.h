#pragma once
#include <string>
#include <vector>
#include "api/feature/command/Command.h"

class Command : public ICommand {
public:
	~Command() = default;
	Command(std::string const& name, std::wstring const& description, std::string const& usage, std::vector<std::string> aliases = {})
		: ICommand(name, description, usage, aliases) {}

	virtual void message(std::wstring const& str, bool error = false) override;
protected:
};
