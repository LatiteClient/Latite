#pragma once
#include "api/feature/module/Module.h"
#include "api/eventing/Event.h"
#include "client/event/Eventing.h"
#include "client/localization/LocalizeString.h"

class Module : public IModule {
public:
	inline static constexpr int nokeybind = 1;

	explicit Module(std::string const& name, std::wstring const& displayName, std::wstring const& description, Category category,
		int keybind = 0, bool hud = false, bool visible = true) : IModule(name, description, displayName, category, visible) {
		this->hud = hud;

		key = KeyValue(keybind);

		{
			auto set = std::make_shared<Setting>("enabled", LocalizeString::get("client.module.props.enabled.name"),
                                                 LocalizeString::get("client.module.props.enabled.desc"));
			set->value = &enabled;
			set->visible = false;
			set->defaultValue = BoolValue(false);
			set->callback = [this](Setting& s) {
				const bool val = std::get<BoolValue>(*s.value);

				if (val != std::get<BoolValue>(enabled))
					this->setEnabled(val);
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

	~Module() {
		Eventing::get().unlisten(this);
	}

	template <typename Event, typename Listener>
	void listen(Listener listener, bool callWhenInactive = false, int priority = 0) {
		Eventing::get().listen<Event>(this, (EventListenerFunc)listener, priority, callWhenInactive);
	}

	[[nodiscard]] virtual bool forceMinecraftRenderer() { return false; };

	virtual void loadConfig(SettingGroup& resolvedGroup) override;
	std::shared_ptr<Setting> addSetting(std::string const& internalName, std::wstring const& displayName, std::wstring const& desc, ValueType& val, Setting::Condition condition = Setting::Condition());
	std::shared_ptr<Setting> addEnumSetting(std::string const& internalName, std::wstring const& displayName, std::wstring const& desc, EnumData& dat, Setting::Condition condition = Setting::Condition());
	std::shared_ptr<Setting> addSliderSetting(std::string const& internalName, std::wstring const& displayName, std::wstring const& desc, ValueType& val, ValueType min, ValueType max, ValueType interval, Setting::Condition condition = Setting::Condition());
protected:
};