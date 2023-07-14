#include "Command.h"
#include "pch.h"

Command::Command(std::string const& name, std::string const& description, std::string const& usage, std::vector<std::string> aliases)
	: cmdName(name), description(description), usage(usage)
{
	this->aliases = { name };
	for (auto& a : aliases) {
		this->aliases.push_back(a);
	}
}
