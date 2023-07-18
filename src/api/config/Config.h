#pragma once
#include <filesystem>
#include <optional>
#include "api/feature/setting/SettingGroup.h"
#include "json/json.hpp"

class Config final {
public:
	explicit Config(std::filesystem::path path);

	// If returns nullopt, operation succeeded. If returns a number, operation unsuccessful.
	std::optional<errno_t> load();
	void addGroup(nlohmann::json obj);
	void addSetting(std::shared_ptr<SettingGroup> group, nlohmann::json& obj);

	void save(std::vector<std::shared_ptr<SettingGroup>> groups);
	void saveGroup(std::shared_ptr<SettingGroup> group);
	void saveSetting(std::shared_ptr<SettingGroup> group, std::shared_ptr<Setting> set);

	std::vector<std::shared_ptr<SettingGroup>> getOutput();
private:
	std::vector<std::shared_ptr<SettingGroup>> groups;
	std::vector<std::shared_ptr<SettingGroup>> out;
	std::filesystem::path path;
};