#pragma once
#include "api/feature/Feature.h"
#include "api/feature/setting/SettingGroup.h"

class IModule : public Listener, public Feature {
public:
	std::shared_ptr<SettingGroup> settings;

	enum Category {
		GAME,
		HUD,
		GAMEHUD,
		SCRIPT,
		SURVIVAL,
	};

	explicit IModule(std::string const& name, std::string const& description,
		std::string const& displayName, Category category, bool visible) : modName(name), description(description), displayName(displayName), category(category), visible(visible) {
		settings = std::make_shared<SettingGroup>(name);
	}
	IModule(IModule&) = delete;
	IModule(IModule&&) = delete;

	virtual ~IModule() = default;
	virtual void onEnable() {};
	virtual void onDisable() {};

	[[nodiscard]] KeyValue getKeybind() { return std::get<KeyValue>(key); }
	[[nodiscard]] bool isEnabled() { return std::get<BoolValue>(enabled); };
	[[nodiscard]] bool isVisible() { return visible; };
	void setEnabled(bool b) { std::get<BoolValue>(enabled) = b; }

	bool shouldListen() { return isEnabled(); }

	virtual void loadConfig(SettingGroup& resolvedGroup) = 0;

	[[nodiscard]] std::string name() override { return modName; }
	[[nodiscard]] std::string desc() override { return description; }
	[[nodiscard]] std::string getDisplayName() { return displayName; }

	[[nodiscard]] Category getCategory() { return category; }
protected:
	std::string modName, description, displayName;
	Setting::Value enabled = BoolValue(false);
	Setting::Value key = KeyValue(0);
	bool visible;
	Category category;
};