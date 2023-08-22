/* Copyright (C) Imrglop, All rights reserved
 * Copyright (C) Latite Client contributors, All rights reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited without the permission of the author.
 * Proprietary and confidential
 * Written by Imrglop <contact: latiteclientgithub@gmail.com>, 2023
 */

#include "ToggleCommand.h"
#include "client/Latite.h"
#include "client/feature/module/ModuleManager.h"
#include <format>

ToggleCommand::ToggleCommand() : Command("toggle", "Toggles a module on or off.", "{0} <module>", {"t"}) {
}

bool ToggleCommand::execute(std::string const label, std::vector<std::string> args) {
	if (args.size() != 1) {
		return false;
	}

	auto mod = Latite::get().getModuleManager().find(args[0]);
	if (mod) {
		mod->setEnabled(!mod->isEnabled());
		message(std::format("Toggled {} {}", mod->getDisplayName(), mod->isEnabled() ? "ON" : "OFF"));
		return true;
	}
	message("Unknown module " + args[0], true);
	return true;
}
