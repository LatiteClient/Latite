#pragma once
#include <string>
#include <vector>
#include "api/feature/Feature.h"

class Command : public Feature {
public:
	Command(std::string const& name, std::string const& description, std::string const& usage, std::vector<std::string> aliases = {});

	virtual bool execute(std::string const label, std::vector<std::string> args) = 0;

	[[nodiscard]] virtual bool isLocalWorldOnly() { return this->localWorldOnly; }
	[[nodiscard]] std::vector<std::string> getAliases() { return aliases; }

	// TODO: implement chat messages
	virtual void message(std::string const& str, bool error = false);
	
	[[nodiscard]] std::string desc() override { return description; };
	[[nodiscard]] std::string name() override { return cmdName; };


protected:
	std::string cmdName, description, usage;
	std::vector<std::string> aliases;
private:
	bool localWorldOnly;
};