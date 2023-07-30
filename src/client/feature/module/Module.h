#pragma once
#include "api/feature/module/Module.h"
#include "api/eventing/Event.h"
#include "client/event/Eventing.h"

class Module : public IModule {
public:
	explicit Module(std::string const& name, std::string const& displayName, std::string const& description, Category category,
		int keybind = 0, bool visible = true) : IModule(name, description, displayName, category, visible) {
			{
				auto set = std::make_shared<Setting>("enabled", "Enabled", "Whether the module is on or not", Setting::Type::Bool);
				set->value = &enabled;
				set->visible = false;

				settings->addSetting(set);
			}
			if (keybind != 1) {
				auto set = std::make_shared<Setting>("key", "Key", "The keybind of the module", Setting::Type::Key);
				set->value = &key;

				settings->addSetting(set);
			}
	}

	template <typename Event, typename Listener>
	void listen(Listener listener, bool callWhenInactive = false, int priority = 0) {
		Eventing::get().listen<Event>(this, (EventListenerFunc)listener, priority, callWhenInactive);
	}

	void loadConfig(SettingGroup& resolvedGroup) override;
	void addSetting(std::string const& internalName, std::string const& displayName, std::string const& desc, Setting::Value& val);
	void addSliderSetting(std::string const& internalName, std::string const& displayName, std::string const& desc, Setting::Value& val, Setting::Value min, Setting::Value max, Setting::Value interval);
protected:
};