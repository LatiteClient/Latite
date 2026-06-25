#pragma once
#include "client/event/Event.h"
#include "client/feature/Feature.h"
#include "client/event/Eventing.h"
#include "client/localization/LocalizeString.h"
#include "client/feature/setting/Setting.h"
#include "client/feature/setting/SettingGroup.h"
#include <optional>

class Module : public Listener, public Feature, public std::enable_shared_from_this<Module>  {
public:
    enum Category {
        GAME,
        HUD,
        GAMEHUD,
        SCRIPT
    };

	inline static constexpr int nokeybind = 1;

	explicit Module(std::string const& name, std::wstring const& displayName, std::wstring const& description, Category category,
		int keybind = 0, bool hud = false, bool visible = true) : settings(std::make_shared<SettingGroup>(name)), modName(name), displayName(displayName), description(description), category(category), visible(visible), hud(hud) {
        settings = std::make_shared<SettingGroup>(name);
		this->hud = hud;

		key = KeyValue(keybind);

		{
			auto set = std::make_shared<Setting>("enabled", LocalizeString::get("client.module.props.enabled.name"),
                                                 LocalizeString::get("client.module.props.enabled.desc"));
			set->value = &enabled;
			set->visible = false;
			set->defaultValue = BoolValue(false);
			set->callback = [this](Setting& s) {
				this->setEnabled(std::get<BoolValue>(*s.value));
			};

			settings->addSetting(set);
		}
		if (keybind != 1) {
			auto set = std::make_shared<Setting>("key", LocalizeString::get("client.module.props.key.name"),
                                                 LocalizeString::get("client.module.props.key.desc"));
			set->value = &key;
			set->defaultValue = KeyValue(keybind);

			settings->addSetting(set);
		}
	}
	explicit Module(std::string const& name, LocalizedString const& displayName, LocalizedString const& description, Category category,
		int keybind = 0, bool hud = false, bool visible = true) : Module(name, displayName.value(), description.value(), category, keybind, hud, visible) {
		setDisplayName(displayName);
		setDescription(description);
	}
	explicit Module(std::string const& name, LocalizedString const& displayName, std::wstring const& description, Category category,
		int keybind = 0, bool hud = false, bool visible = true) : Module(name, displayName.value(), description, category, keybind, hud, visible) {
		setDisplayName(displayName);
	}
	explicit Module(std::string const& name, std::wstring const& displayName, LocalizedString const& description, Category category,
		int keybind = 0, bool hud = false, bool visible = true) : Module(name, displayName, description.value(), category, keybind, hud, visible) {
		setDescription(description);
	}

    Module(Module&) = delete;
    Module(Module&&) = delete;

	virtual ~Module() {
		Eventing::get().unlisten(this);
	}

	template <typename Event, typename Listener>
	void listen(Listener listener, bool callWhenInactive = false, int priority = 0) {
		Eventing::get().listen<Event>(this, (EventListenerFunc)listener, priority, callWhenInactive);
	}

	virtual void onEnable() {};
	virtual void onDisable() {};
    virtual void onInit() {};
	virtual bool isToggleable() { return true; }
	virtual bool shouldPersistEnabled() { return true; }

    [[nodiscard]] KeyValue getKeybind() { return std::get<KeyValue>(key); }
    [[nodiscard]] bool isEnabled() { return std::get<BoolValue>(enabled); };
    [[nodiscard]] bool isHud() const { return hud; };
    [[nodiscard]] bool isTextual() const { return textual; };
    [[nodiscard]] bool isVisible() const { return visible; };
    [[nodiscard]] bool isBlocked() const { return blocked; };

	virtual bool forceMinecraftRenderer() { return false; };
    virtual void afterLoadConfig() {}
    virtual void loadConfig(SettingGroup& resolvedGroup);
    virtual bool shouldHoldToToggle() { return false; }
	bool shouldListen() override { return this->isEnabled(); };

    void setEnabled(bool b, bool blockedOverride = false) {
		if (!blockedOverride && isBlocked()) return;
		b ? onEnable() : onDisable();
		std::get<BoolValue>(enabled) = shouldPersistEnabled() ? b : false;
	}
    void setBlocked(bool b) {
        if (b) {
            if (isEnabled()) {
                setEnabled(false);
            }
        }

        blocked = b;
    }


    [[nodiscard]] std::string name() override { return modName; }
    [[nodiscard]] std::wstring desc() override { return description; }
    [[nodiscard]] std::wstring getDisplayName() { return displayName; }

	virtual void refreshLocalization() {
		if (displayNameKey) displayName = LocalizeString::get(*displayNameKey).value();
		if (descriptionKey) description = LocalizeString::get(*descriptionKey).value();
		settings->refreshLocalization();
	}

    [[nodiscard]] Category getCategory() { return category; }

	std::shared_ptr<Setting> addSetting(std::string const& internalName, std::wstring const& displayName, std::wstring const& desc, ValueType& val, Setting::Condition condition = Setting::Condition());
	std::shared_ptr<Setting> addSetting(std::string const& internalName, LocalizedString const& displayName, std::wstring const& desc, ValueType& val, Setting::Condition condition = Setting::Condition());
	std::shared_ptr<Setting> addSetting(std::string const& internalName, std::wstring const& displayName, LocalizedString const& desc, ValueType& val, Setting::Condition condition = Setting::Condition());
	std::shared_ptr<Setting> addSetting(std::string const& internalName, LocalizedString const& displayName, LocalizedString const& desc, ValueType& val, Setting::Condition condition = Setting::Condition());
	std::shared_ptr<Setting> addEnumSetting(std::string const& internalName, std::wstring const& displayName, std::wstring const& desc, EnumData& dat, Setting::Condition condition = Setting::Condition());
	std::shared_ptr<Setting> addEnumSetting(std::string const& internalName, LocalizedString const& displayName, std::wstring const& desc, EnumData& dat, Setting::Condition condition = Setting::Condition());
	std::shared_ptr<Setting> addEnumSetting(std::string const& internalName, std::wstring const& displayName, LocalizedString const& desc, EnumData& dat, Setting::Condition condition = Setting::Condition());
	std::shared_ptr<Setting> addEnumSetting(std::string const& internalName, LocalizedString const& displayName, LocalizedString const& desc, EnumData& dat, Setting::Condition condition = Setting::Condition());
	std::shared_ptr<Setting> addSliderSetting(std::string const& internalName, std::wstring const& displayName, std::wstring const& desc, ValueType& val, ValueType min, ValueType max, ValueType interval, Setting::Condition condition = Setting::Condition());
	std::shared_ptr<Setting> addSliderSetting(std::string const& internalName, LocalizedString const& displayName, std::wstring const& desc, ValueType& val, ValueType min, ValueType max, ValueType interval, Setting::Condition condition = Setting::Condition());
	std::shared_ptr<Setting> addSliderSetting(std::string const& internalName, std::wstring const& displayName, LocalizedString const& desc, ValueType& val, ValueType min, ValueType max, ValueType interval, Setting::Condition condition = Setting::Condition());
	std::shared_ptr<Setting> addSliderSetting(std::string const& internalName, LocalizedString const& displayName, LocalizedString const& desc, ValueType& val, ValueType min, ValueType max, ValueType interval, Setting::Condition condition = Setting::Condition());

    std::shared_ptr<SettingGroup> settings;
protected:
	void setDisplayName(LocalizedString const& text) {
		displayName = text.value();
		displayNameKey = text.key();
	}

	void setDescription(LocalizedString const& text) {
		description = text.value();
		descriptionKey = text.key();
	}

    std::string modName;
    std::wstring description, displayName;
	std::optional<std::string> descriptionKey;
	std::optional<std::string> displayNameKey;
    ValueType enabled = BoolValue(false);
    ValueType key = KeyValue(0);
    bool visible;
    bool hud = false;
    bool textual = false;
    bool blocked = false;
    Category category;
private:

};
