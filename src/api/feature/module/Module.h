#pragma once
#include "api/feature/Feature.h"
#include "api/feature/setting/SettingGroup.h"

class Module : public Feature, public SettingGroup {
public:
	explicit Module(std::string const& name, std::string const& description, 
		std::string const& displayName) : modName(name), description(description) {
	}

	[[nodiscard]] std::string name() override { return modName; }
	[[nodiscard]] std::string desc() override { return description; }
	[[nodiscard]] std::string getDisplayName() { return displayName; }
protected:
	std::string modName, description, displayName;
};