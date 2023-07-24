#include "ConfigManager.h"
#include "util/util.h"
#include "util/Logger.h"
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

bool ConfigManager::saveCurrentConfig() {
	return save(loadedConfig);
}

bool ConfigManager::load(std::shared_ptr<Config> cfg) {
	loadedConfig = cfg;
	auto res = cfg->load();
	if (res != std::nullopt) return false;
	for (auto& item : cfg->getOutput()) {

		// Might be a bit hacky
		if (Latite::getSettings().name() == item->name()) {
			Latite::get().loadConfig(*item.get());
		}
		else {
			auto mod = Latite::getModuleManager().find(item->name());
			if (!mod) {
				Logger::Warn("Could not find {} as module in config", item->name());
			}
			else {
				mod->loadConfig(*item.get());
			}
		}
	}
	return true;
}

bool ConfigManager::save(std::shared_ptr<Config> cfg) {
	std::vector<SettingGroup*> groups = {};
	groups.push_back(&Latite::get().getSettings());

	Latite::getModuleManager().forEach([&](std::shared_ptr<IModule> mod) {
		groups.push_back(mod->settings.get());
		});

	auto res = cfg->save(groups);
	return res == std::nullopt;
}
