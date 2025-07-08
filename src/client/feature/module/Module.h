#pragma once
#include "client/event/Event.h"
#include "client/feature/Feature.h"
#include "client/event/Eventing.h"
#include "client/localization/LocalizeString.h"
#include "client/feature/setting/Setting.h"
#include "client/feature/setting/SettingGroup.h"

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
		int keybind = 0, bool hud = false, bool visible = true) : settings(std::make_shared<SettingGroup>(name)) {
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

    void setEnabled(bool b, bool blockedOverride = false) { if (!blockedOverride && isBlocked()) return; b ? onEnable() : onDisable(); std::get<BoolValue>(enabled) = b; }
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

    [[nodiscard]] Category getCategory() { return category; }

	std::shared_ptr<Setting> addSetting(std::string const& internalName, std::wstring const& displayName, std::wstring const& desc, ValueType& val, Setting::Condition condition = Setting::Condition());
	std::shared_ptr<Setting> addEnumSetting(std::string const& internalName, std::wstring const& displayName, std::wstring const& desc, EnumData& dat, Setting::Condition condition = Setting::Condition());
	std::shared_ptr<Setting> addSliderSetting(std::string const& internalName, std::wstring const& displayName, std::wstring const& desc, ValueType& val, ValueType min, ValueType max, ValueType interval, Setting::Condition condition = Setting::Condition());

    std::shared_ptr<SettingGroup> settings;
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
private:

};