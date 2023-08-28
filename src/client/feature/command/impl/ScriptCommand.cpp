#include "pch.h"
#include "ScriptCommand.h"
#include "client/script/ScriptManager.h"
#include <filesystem>
#include "json/json.hpp"
#include "../CommandManager.h"
#include "client/Latite.h"



using namespace winrt::Windows::Storage::Streams;
using namespace winrt::Windows::Web::Http;
using namespace winrt::Windows::Web::Http::Filters;

ScriptCommand::ScriptCommand() : Command("script", "Do actions related to scripts", "\n$ load <folderPath>\n$ install <scriptName>\n$ unload <scriptName>\n$ reload <scriptName>\n$ startup <folderPath>\n$ unload all", {})
{
}

bool ScriptCommand::execute(std::string const label, std::vector<std::string> args)
{
	if (args.empty()) return false;
	if (!ScriptManager::scriptingSupported()) {
		message("&eScripting is not supported!");
		return true;
	}

	if (args[0] == "load") {
		if (args.size() != 2) return false;
		auto res = Latite::getScriptManager().loadScript(util::StrToWStr(args[1]), true);
		if (res) {
			std::wstringstream ss;
			ss << "Loaded script " << res->data.name << " " << res->data.version << "!";
			message(util::WStrToStr(ss.str()));
		}
		else {
			message("Could not load and run the script successfully! Check your folder path.");
		}
	}
	else if (args[0] == "unload") {
		if (args.size() != 2) return false;
		if (args[1] == "all") {
			Latite::getScriptManager().unloadAll();
			message("Unloaded all scripts.");
			return true;
		}
		else {
			if (auto script = Latite::getScriptManager().getScriptByName(util::StrToWStr(args[1]))) {
				Latite::getScriptManager().unloadScript(script);
				message("Successfully unloaded script.");
				return true;
			}
			message("Unknown script " + args[1], true);
			return true;
		}
	}
	else if (args[0] == "startup") {
		if (args.size() != 2) return false;
		auto& scr = args[1];
		auto path = scr;
		if (!std::filesystem::exists(path))
			path = (util::GetLatitePath() / ("Scripts") / scr).string();
		if (std::filesystem::exists(path)) {
			std::filesystem::rename(path, util::GetLatitePath() / "Scripts" / "Startup" / (std::filesystem::path(path).filename().string()));
			message("Successfully moved script folder " + scr + " to startup.");
			return true;
		}

		message("Unknown script " + args[1], true);
		return true;
	}
	else if (args[0] == "reload") {
		if (args.size() != 2) return false;
		if (auto script = Latite::getScriptManager().getScriptByName(util::StrToWStr(args[1]))) {
			auto path = script->relFolderPath;
			Latite::getScriptManager().unloadScript(script);
			auto sc = Latite::getScriptManager().loadScript(path, true);
			if (sc) message("Successfully reloaded script.");
			else message("Could not reload script properly.", true);
			return true;
		}
		message("Unknown script " + args[1], true);
		return true;

	}
	else if (args[0] == "install") {
		if (args.size() != 2) return false;
		auto err = Latite::getScriptManager().installScript(args[1]);
		if (err.has_value()) {
			message("Could not install script (" + err.value() + std::string(")"), true);
			return true;
		}
		message("Script installed. Do &7" + Latite::getCommandManager().prefix + "script load &7Startup/" + args[1] + "&r to run the script.");
		message("This script will load every time you load Minecraft. To disable this, move the script out of the &7Startup&r folder.");
		return true;
	}
	else {
		return false;
	}
	return true;
}
