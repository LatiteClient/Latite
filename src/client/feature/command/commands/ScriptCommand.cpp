#include "pch.h"
#include "ScriptCommand.h"
#include "client/script/PluginManager.h"
#include <filesystem>
#include <nlohmann/json.hpp>
#include "../CommandManager.h"
#include "client/Latite.h"
#include <client/script/JsPlugin.h>

using namespace winrt::Windows::Storage::Streams;
using namespace winrt::Windows::Web::Http;
using namespace winrt::Windows::Web::Http::Filters;

ScriptCommand::ScriptCommand() : Command("plugin", LocalizeString::get("client.commands.plugin.desc"),
                                         "\n$ load <folderPath>\n$ install <pluginName>\n$ unload \"<pluginName>\"\n$ startup <folderPath>\n$ unload all",
                                         {"script"}) {
}

bool ScriptCommand::execute(std::string const label, std::vector<std::string> args)
{
	if (args.empty()) return false;
	if (!PluginManager::scriptingSupported()) {
		message(util::WFormat(LocalizeString::get("client.commands.plugin.scriptingNotSupported.name")));
		return true;
	}

	if (args[0] == "load") {
		if (args.size() != 2) return false;
		auto res = Latite::getPluginManager().loadPlugin(util::StrToWStr(args[1]), true);
		if (res) {
			message(util::FormatWString(LocalizeString::get("client.commands.plugin.load.success.name"),
                                        { res->getName(), res->getVersion() }));
		}
		else {
			message(LocalizeString::get("client.commands.plugin.load.error.name"));
		}
	}
	else if (args[0] == "unload") {
		if (args.size() != 2) return false;
		if (args[1] == "all") {
			Latite::getPluginManager().unloadAll();
			message(LocalizeString::get("client.commands.plugin.unload.all.name"));
			return true;
		}
		else {
			if (auto script = Latite::getPluginManager().getPluginByName(util::StrToWStr(args[1]))) {
				Latite::getPluginManager().popScript(script);
				message(LocalizeString::get("client.commands.plugin.unload.name"));
				return true;
			}
			message(util::FormatWString(LocalizeString::get("client.commands.plugin.unload.unknownPlugin.name"),
				{ util::StrToWStr(args[1]) }), true);
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
			message(util::FormatWString(LocalizeString::get("client.commands.plugin.startup.name"),
                                        { util::StrToWStr(scr) }));
			return true;
		}

		message(util::FormatWString(LocalizeString::get("client.commands.plugin.startup.error.name"),
                                    { util::StrToWStr(args[1]) }), true);
		return true;
	}
	else if (args[0] == "install") {
		if (args.size() != 2) return false;
		auto err = Latite::getPluginManager().installScript(args[1]);
		if (err.has_value()) {
			return true;
		}
		message(util::WFormat(util::FormatWString(LocalizeString::get("client.commands.plugin.install.name"), {
                                                      util::StrToWStr(Latite::getCommandManager().prefix),
                                                      util::StrToWStr(args[1])
                                                  })));
		return true;
	}
	else {
		return false;
	}
	return true;
}
