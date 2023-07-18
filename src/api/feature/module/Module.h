#pragma once
#include "api/feature/Feature.h"
#include "api/feature/setting/SettingGroup.h"
#include "api/eventing/Event.h"

class IModule : public Feature, public SettingGroup {
public:
	explicit IModule(std::string const& name, std::string const& description, 
		std::string const& displayName) : SettingGroup(name), modName(name), description(description), displayName(displayName) {
		auto set = std::make_shared<Setting>("enabled", "", Setting::Type::Bool);
		set->value = &enabled;
		addSetting(set);
	}

	virtual ~IModule() = default;
	virtual void onEnable() {};
	virtual void onDisable() {};

	[[nodiscard]] bool isEnabled() { return std::get<BoolValue>(enabled); };
	void setEnabled(bool b) { std::get<BoolValue>(enabled) = b; }

	[[nodiscard]] std::string name() override { return modName; }
	[[nodiscard]] std::string desc() override { return description; }
	[[nodiscard]] std::string getDisplayName() { return displayName; }
protected:
	std::string modName, description, displayName;
	Setting::Value enabled = BoolValue(false);
};