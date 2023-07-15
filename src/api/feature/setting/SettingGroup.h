#pragma once
#include <vector>
#include <memory>
#include <functional>

#include "Setting.h"

class SettingGroup {
public:
	void forEach(std::function<void(std::shared_ptr<Setting> set)> callback) {
		for (auto& setting : settings) {
			callback(setting);
		}
	}

	inline void addSetting(std::shared_ptr<Setting> set) {
		settings.push_back(set);
	}
protected:
	std::vector<std::shared_ptr<Setting>> settings;
};