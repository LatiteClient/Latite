#include "pch.h"
#include "ConfigManager.h"
#include "client/Latite.h"
#include "client/feature/module/ModuleManager.h"

ConfigManager::ConfigManager() {
	auto folder = util::GetLatitePath() / "Configs";
	std::filesystem::create_directory(folder);
	auto path = folder / "default.json";
	masterConfig = std::make_shared<Config>(path);
}

bool ConfigManager::loadMaster() {
	return load(masterConfig);
}

void ConfigManager::applyLanguageConfig(std::string_view languageSettingName) {
	for (auto& item : loadedConfig->getOutput()) {

		// Might be a bit hacky
		if (Latite::getSettings().name() == item->name()) {
			item->forEach([&](std::shared_ptr<Setting> set) {
				if (set->name() == languageSettingName) {
					Latite::get().loadLanguageConfig(set);
				}
				});
			Latite::get().loadConfig(*item.get());
		}
	}
}

void ConfigManager::applyGlobalConfig() {
	for (auto& item : loadedConfig->getOutput()) {

		// Might be a bit hacky
		if (Latite::getSettings().name() == item->name()) {
			Latite::get().loadConfig(*item.get());
		}
	}
}

void ConfigManager::applyModuleConfig() {
	for (auto& item : loadedConfig->getOutput()) {
		auto mod = Latite::getModuleManager().find(item->name());
		if (!mod) {
			Logger::Warn("Could not find {} as module in config", item->name());
		}
		else {
			mod->loadConfig(*item.get());
		}
	}
}

bool ConfigManager::saveCurrentConfig() {
	return save(loadedConfig);
}

bool ConfigManager::saveTo(std::wstring const& name) {
	std::filesystem::path path = getUserPath() / (name + L".json");

	for (auto& cfg : items) {
		if (path == cfg->getPath()) {
			return save(cfg);
		}
	}

	auto cfg = std::make_shared<Config>(path);
	return save(cfg);
}


bool ConfigManager::loadUserConfig(std::wstring const& name) {
	auto path = getUserPath() / name;
	if (!std::filesystem::exists(path)) path += ".json";
	if (!std::filesystem::exists(path)) {
		return false;
	}
	auto config = std::make_shared<Config>(path);
	return load(std::move(config));
}

std::filesystem::path ConfigManager::getUserPath() {
	return util::GetLatitePath() / "Configs";
}

bool ConfigManager::load(std::shared_ptr<Config> cfg) {
	loadedConfig = cfg;
	auto res = cfg->load();
	if (res != std::nullopt) return false;
	return true;
}

bool ConfigManager::save(std::shared_ptr<Config> cfg) {
	std::vector<SettingGroup*> groups = {};
	groups.push_back(&Latite::getSettings());

	Latite::getModuleManager().forEach([&](std::shared_ptr<Module> mod) {
		groups.push_back(mod->settings.get());
		});

	auto res = cfg->save(groups);
	return res == std::nullopt;
}
