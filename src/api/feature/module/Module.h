#pragma once
#include "api/feature/Feature.h"
#include "api/feature/setting/SettingGroup.h"

class IModule : public Listener, public Feature {
public:
	std::shared_ptr<SettingGroup> settings;

	explicit IModule(std::string const& name, std::string const& description, 
		std::string const& displayName) : modName(name), description(description), displayName(displayName) {
		auto set = std::make_shared<Setting>("enabled", "", Setting::Type::Bool);
		set->value = &enabled;

		settings = std::make_shared<SettingGroup>(name);
		settings->addSetting(set);
	}

	virtual ~IModule() = default;
	virtual void onEnable() {};
	virtual void onDisable() {};

	[[nodiscard]] bool isEnabled() { return std::get<BoolValue>(enabled); };
	void setEnabled(bool b) { std::get<BoolValue>(enabled) = b; }

	bool shouldListen() { return isEnabled(); }

	virtual void loadConfig(SettingGroup& resolvedGroup) = 0;

	[[nodiscard]] std::string name() override { return modName; }
	[[nodiscard]] std::string desc() override { return description; }
	[[nodiscard]] std::string getDisplayName() { return displayName; }
protected:
	std::string modName, description, displayName;
	Setting::Value enabled = BoolValue(false);
};