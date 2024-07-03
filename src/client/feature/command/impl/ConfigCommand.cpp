#include "pch.h"
#include "ConfigCommand.h"
#include "client/config/ConfigManager.h"
#include "client/Latite.h"

ConfigCommand::ConfigCommand() : Command("config", LocalizeString::get("client.commands.config.desc"), "$ load <name>\n$ save [name]", {"profile", "configs", "profiles", "cfg"}) {
}

bool ConfigCommand::execute(std::string const label, std::vector<std::string> args) {
	if (args.empty()) return false;
	if (args[0] == "save") {
		if (args.size() < 2) {
			if (Latite::getConfigManager().saveCurrentConfig())
				message(LocalizeString::get("client.commands.config.savedConfig"));
			else message(LocalizeString::get("client.commands.config.genericError"), true);
			return true;
		}
		if (Latite::getConfigManager().saveTo(util::StrToWStr(args[1]))) {
			message(util::FormatWString(LocalizeString::get("client.commands.config.savedConfigPath"), { util::StrToWStr(args[1]) }));
			return true;
		}
		message(LocalizeString::get("client.commands.config.genericError"), true);
		return true;
	}
	else if (args[0] == "load") {
		if (args.size() < 2) {
			return false;
		}

		if (!Latite::getConfigManager().saveCurrentConfig()) {
			message(LocalizeString::get("client.commands.config.saveDuringLoadingError"), true);
			return true;
		}

		if (!Latite::getConfigManager().loadUserConfig(util::StrToWStr(args[1]))) {
			message(util::FormatWString(LocalizeString::get("client.commands.config.configNotFound"), { util::StrToWStr(args[1]) }), true);
			LocalizeString::get("");
			return true;
		}
		message(util::FormatWString(LocalizeString::get("client.commands.config.loadedConfig"), { util::StrToWStr(args[1]) }));
		return true;
	}
	return false;
}
