#include "SetPrefixCommand.h"
#include "../CommandManager.h"
#include "client/Latite.h"
#include "pch.h"

SetPrefixCommand::SetPrefixCommand() : Command("setprefix", "Set the command prefix", "{0} [prefix]") {
}

bool SetPrefixCommand::execute(std::string const label, std::vector<std::string> args) {
	if (args.empty()) {
		message("The command prefix is set to &7'" + Latite::getCommandManager().prefix + "'");
		return true;
	}
	else {
		Latite::getCommandManager().prefix = args[0];
		message("Set the command prefix to &7'" + args[0] + "'");
		return true;
	}
	return false;
}
