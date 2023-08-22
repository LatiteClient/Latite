/* Copyright (C) Imrglop, All rights reserved
 * Copyright (C) Latite Client contributors, All rights reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited without the permission of the author.
 * Proprietary and confidential
 * Written by Imrglop <contact: latiteclientgithub@gmail.com>, 2023
 */

#include "SetPrefixCommand.h"
#include "../CommandManager.h"
#include "client/Latite.h"

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
