#include "pch.h"
#include "ToggleCommand.h"
#include "client/Latite.h"
#include "client/feature/module/ModuleManager.h"

ToggleCommand::ToggleCommand() : Command("toggle", LocalizeString::get("client.commands.toggle.desc"), "$ <module>",
                                         { "t" }) {
}

bool ToggleCommand::execute(std::string const label, std::vector<std::string> args) {
	if (args.size() != 1) {
		return false;
	}

	auto mod = Latite::getModuleManager().find(args[0]);
	if (mod) {
		mod->setEnabled(!mod->isEnabled());
		message(util::StrToWStr(std::format("Toggled {} {}", mod->getDisplayName(), mod->isEnabled() ? "ON" : "OFF")));
		return true;
	}
	message(util::FormatWString(LocalizeString::get("client.commands.toggle.msg.unknownModule.name"),
                                { util::StrToWStr(args[0]) }), true);
	return true;
}
