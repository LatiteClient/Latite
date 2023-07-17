#pragma once
#include <vector>
#include <memory>
#include <functional>

#include "Setting.h"

// A group of settings.
class SettingGroup {
public:
	explicit SettingGroup(std::string const& name) : groupName(name) {}
	~SettingGroup() = default;

	void forEach(std::function<void(std::shared_ptr<Setting> set)> callback) {
		for (auto& setting : settings) {
			callback(setting);
		}
	}

	inline void addSetting(std::shared_ptr<Setting> set) {
		settings.push_back(set);
	}

protected:
	std::string groupName;
	std::vector<std::shared_ptr<Setting>> settings;
};