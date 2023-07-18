#include "ToggleCommand.h"
#include "client/Latite.h"
#include "client/feature/module/ModuleManager.h"
#include <format>

ToggleCommand::ToggleCommand() : Command("toggle", "Toggles a module on or off.", "<module>", {"t"}) {
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
