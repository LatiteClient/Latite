#include "ScriptCommand.h"
#include "client/script/ScriptManager.h"
#include <filesystem>
#include "json/json.hpp"
#include "../CommandManager.h"
#include "util/Util.h"
#include "client/Latite.h"

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Web.Http.h>
#include <winrt/impl/windows.web.http.2.h>
#include <winrt/Windows.Web.Http.Filters.h>
#include <winrt/windows.foundation.collections.h>
#include <winrt/impl/windows.foundation.collections.1.h>
#include <winrt/Windows.Web.Http.Headers.h>


using namespace winrt::Windows::Storage::Streams;
using namespace winrt::Windows::Web::Http;
using namespace winrt::Windows::Web::Http::Filters;

ScriptCommand::ScriptCommand() : Command("script", "Do actions related to scripts", "\nload <folderPath>\ninstall <scriptName>\nunload <scriptName>\nreload <scriptName>\nstartup <folderPath>\nunload all", {})
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
		auto res = Latite::getScriptManager().loadScript(args[1], true);
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
			if (auto script = Latite::getScriptManager().getScriptByName(args[1])) {
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
		if (auto script = Latite::getScriptManager().getScriptByName(args[1])) {
			auto path = script->relFolderPath;
			Latite::getScriptManager().unloadScript(script);
			auto sc = Latite::getScriptManager().loadScript(util::WStrToStr(path), true);
			if (sc) message("Successfully reloaded script.");
			else message("Could not reload script properly.", true);
			return true;
		}
		message("Unknown script " + args[1], true);
		return true;

	}
	else if (args[0] == "install") {
		if (args.size() != 2) return false;
		std::wstring token = L"?token=GHSAT0AAAAAACETEXYVRCL7YLMVSDOYZB6OZFLNT7Q";
		std::wstring registry = L"https://raw.githubusercontent.com/LatiteScripting/Scripts/master/registry/";
		std::wstring jsonPath = registry + L"/scripts.json" + token;
		nlohmann::json scriptsJson;
		auto http = HttpClient();
		{
			// get JSON
			winrt::Windows::Foundation::Uri requestUri(jsonPath);

			HttpRequestMessage request(HttpMethod::Get(), requestUri);

			try {
				auto operation = http.SendRequestAsync(request);
				auto response = operation.get();
				auto cont = response.Content();
				auto strs = cont.ReadAsStringAsync().get();

				try {
					scriptsJson = nlohmann::json::parse(std::wstring(strs.c_str()));
				}
				catch (nlohmann::json::parse_error& e) {
					message(e.what(), true);
					return true;
				}
			}
			catch (winrt::hresult_error const& err) {
				message(util::WStrToStr(err.message().c_str()), true);
				return true;
			}
		}
		auto arr = scriptsJson["scripts"];
		for (auto& js : arr) {
			auto name = js["name"].get<std::string>();
			auto oName = js["name"].get<std::string>();
			auto woName = util::StrToWStr(oName);
			std::string in = args[1];
			std::transform(in.begin(), in.end(), in.begin(), ::tolower);
			std::transform(name.begin(), name.end(), name.begin(), ::tolower);
			if (in == name) {
				message("Installing " + oName + " v" + js["version"].get<std::string>() + " by " + js["author"].get<std::string>());
				std::wstring path = util::GetLatitePath() / "Scripts" / woName;
				std::filesystem::create_directory(path);
				for (auto& fil : js["files"]) {
					auto fws = util::StrToWStr(fil.get<std::string>());
					winrt::Windows::Foundation::Uri requestUri(registry + woName + L"/" + fws + token);
					HttpRequestMessage request(HttpMethod::Get(), requestUri);
					auto operation = http.SendRequestAsync(request);
					auto response = operation.get();
					auto cont = response.Content();
					auto strs = cont.ReadAsStringAsync().get();
					std::wofstream ofs;
					ofs.open(path + L"\\" + fws);
					if (ofs.fail()) {
						message("Error opening file: " + std::to_string(*_errno()), true);
						return true;
					}
					ofs << strs.c_str();
					ofs.close();
				}
				message("Script installed. Do &7" + Latite::getCommandManager().prefix + "script load &7" + oName + "&r to run the script.");
				return true;
			}
		}
		message("Could not find script " + args[0], true);
		return true;
	}
	else {
		return false;
	}
	return true;
}
