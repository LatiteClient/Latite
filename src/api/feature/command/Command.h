#pragma once
#include <string>
#include <vector>
#include "api/feature/Feature.h"

class ICommand : public Feature {
public:
	ICommand(std::string const& name, std::string const& description, std::string const& usage, std::vector<std::string> aliases = {})
		: cmdName(name), description(description), usage(usage)
	{
		this->aliases = { name };
		for (auto& a : aliases) {
			this->aliases.push_back(a);
		}
	}
	~ICommand() = default;

	virtual bool execute(std::string const label, std::vector<std::string> args) = 0;

	[[nodiscard]] virtual bool isLocalWorldOnly() { return this->localWorldOnly; }
	[[nodiscard]] std::vector<std::string> getAliases() { return aliases; }
	[[nodiscard]] std::string getUsage() { return usage; }

	virtual void message(std::string const& str, bool error = false) = 0;
	
	[[nodiscard]] std::string desc() override { return description; };
	[[nodiscard]] std::string name() override { return cmdName; };


protected:
	std::string cmdName, description, usage;
	std::vector<std::string> aliases;
private:
	bool localWorldOnly;
};