#include "pch.h"
#include "ScriptCommand.h"
#include "client/script/PluginManager.h"
#include <filesystem>
#include "json/json.hpp"
#include "../CommandManager.h"
#include "client/Latite.h"
#include <client/script/JsPlugin.h>

using namespace winrt::Windows::Storage::Streams;
using namespace winrt::Windows::Web::Http;
using namespace winrt::Windows::Web::Http::Filters;

ScriptCommand::ScriptCommand() : Command("plugin", "Do actions related to plugins/scripts", "\n$ load <folderPath>\n$ install <pluginName>\n$ unload <pluginName>\n$ startup <folderPath>\n$ unload all", {"script"})
{
}

bool ScriptCommand::execute(std::string const label, std::vector<std::string> args)
{
	if (args.empty()) return false;
	if (!PluginManager::scriptingSupported()) {
		message("&eScripting/Plugins are not supported! Try restarting your game.");
		return true;
	}

	if (args[0] == "load") {
		if (args.size() != 2) return false;
		auto res = Latite::getPluginManager().loadPlugin(util::StrToWStr(args[1]), true);
		if (res) {
			std::wstringstream ss;
			ss << "Loaded plugin " << res->getName() << " " << res->getVersion() << "!";
			message(util::WStrToStr(ss.str()));
		}
		else {
			message("Could not load and run the plugin successfully! Check your folder path.");
		}
	}
	else if (args[0] == "unload") {
		if (args.size() != 2) return false;
		if (args[1] == "all") {
			Latite::getPluginManager().unloadAll();
			message("Unloaded all plugins.");
			return true;
		}
		else {
			if (auto script = Latite::getPluginManager().getPluginByName(util::StrToWStr(args[1]))) {
				Latite::getPluginManager().popScript(script);
				message("Successfully unloaded plugin.");
				return true;
			}
			message("Unknown plugin " + args[1], true);
			return true;
		}
	}
	else if (args[0] == "startup") {
		if (args.size() != 2) return false;
		auto& scr = args[1];
		auto path = scr;
		if (!std::filesystem::exists(path))
			path = (util::GetLatitePath() / ("Plugins") / scr).string();
		if (std::filesystem::exists(path)) {
			std::filesystem::rename(path, PluginManager::getUserPrerunDir() / (std::filesystem::path(path).filename().string()));
			message("Successfully moved plugin folder " + scr + " to startup.");
			return true;
		}

		message("Cannot find plugin " + args[1], true);
		return true;
	}
	else if (args[0] == "install") {
		if (args.size() != 2) return false;
		auto err = Latite::getPluginManager().installScript(args[1]);
		if (err.has_value()) {
			return true;
		}
		message("plugin installed. Do &7" + Latite::getCommandManager().prefix + "plugin load &7" + args[1] + "&r to run the plugin.");
		message("This plugin will load every time you load Minecraft.");
		return true;
	}
	else {
		return false;
	}
	return true;
}
