#pragma once
#include "api/feature/Feature.h"
#include "api/feature/setting/SettingGroup.h"
#include <memory>

class IModule : public Listener, public Feature, public std::enable_shared_from_this<IModule> {
public:
	std::shared_ptr<SettingGroup> settings;

	enum Category {
		GAME,
		HUD,
		GAMEHUD,
		SCRIPT,
		SURVIVAL,
	};

	explicit IModule(std::string const& name, std::wstring const& description,
		std::wstring const& displayName, Category category, bool visible) : modName(name), description(description), displayName(displayName), category(category), visible(visible) {
		settings = std::make_shared<SettingGroup>(name);
	}
	IModule(IModule&) = delete;
	IModule(IModule&&) = delete;

	virtual ~IModule() = default;
	virtual void onEnable() {};
	virtual void onDisable() {};
	virtual void onInit() {};

	[[nodiscard]] KeyValue getKeybind() { return std::get<KeyValue>(key); }
	[[nodiscard]] bool isEnabled() { return std::get<BoolValue>(enabled); };
	[[nodiscard]] bool isHud() const { return hud; };
	[[nodiscard]] bool isTextual() const { return textual; };
	[[nodiscard]] bool isVisible() const { return visible; };
	[[nodiscard]] bool isBlocked() const { return blocked; };
	void setEnabled(bool b, bool blockedOverride = false) {
		if (!blockedOverride && isBlocked()) {
			return;
		}
		b ? onEnable() : onDisable();
		std::get<BoolValue>(enabled) = b;
		shouldListen = b;
	}
	void setBlocked(bool b) {
		if (b) {
			if (isEnabled()) {
				setEnabled(false);
			}
		}

		blocked = b;
	}

	virtual void afterLoadConfig() {}
	virtual void loadConfig(SettingGroup& resolvedGroup) = 0;
	virtual bool shouldHoldToToggle() { return false; }

	bool isListening() override { return this->isEnabled(); };

	[[nodiscard]] std::string name() override { return modName; }
	[[nodiscard]] std::wstring desc() override { return description; }
	[[nodiscard]] std::wstring getDisplayName() { return displayName; }

	[[nodiscard]] Category getCategory() { return category; }

	std::shared_ptr<IModule> getShared() {
		return shared_from_this();
	}
protected:
	std::string modName;
	std::wstring description, displayName;
	ValueType enabled = BoolValue(false);
	ValueType key = KeyValue(0);
	bool visible;
	bool hud = false;
	bool textual = false;
	bool blocked = false;
	Category category;
};