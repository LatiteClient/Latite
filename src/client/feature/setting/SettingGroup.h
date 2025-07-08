#pragma once
#include <vector>
#include <memory>
#include <functional>

#include "Setting.h"

// A group of settings.
class SettingGroup : public std::enable_shared_from_this<SettingGroup> {
public:
	explicit SettingGroup(std::string const& name) : groupName(name) {}
	~SettingGroup() = default;

	void forEach(std::function<void(std::shared_ptr<Setting> set)> callback) {
		for (auto& setting : settings) {
			callback(setting);
		}
	}

	[[nodiscard]] size_t size() {
		return settings.size();
	}

	inline void addSetting(std::shared_ptr<Setting> set) {
		settings.push_back(set);
	}

	[[nodiscard]] std::string name() { return groupName; }

	// TODO: Is this needed?
	[[nodiscard]] std::shared_ptr<SettingGroup> getShared() {
		return shared_from_this();
	}

protected:
	std::string groupName;
	std::vector<std::shared_ptr<Setting>> settings;
};