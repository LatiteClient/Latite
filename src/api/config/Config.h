#pragma once
#include <filesystem>
#include <optional>
#include "api/feature/setting/SettingGroup.h"
#include "json/json.hpp"

class Config {
public:
	explicit Config(std::filesystem::path path);

	// If returns nullopt, operation succeeded. If returns a number, operation unsuccessful.
	std::optional<errno_t> load();
	void addGroup(nlohmann::json obj);
	void addSetting(SettingGroup& group, nlohmann::json& obj);

	std::optional<errno_t> save(std::vector<SettingGroup*> groups);
	void saveGroup(SettingGroup& group, nlohmann::json& obj);
	void saveSetting(std::shared_ptr<Setting> set, nlohmann::json& obj);

	[[nodiscard]] std::vector<std::shared_ptr<SettingGroup>> getOutput() noexcept;
private:
	std::vector<std::shared_ptr<SettingGroup>> groups;
	std::vector<std::shared_ptr<SettingGroup>> out;
	std::filesystem::path path;
};