/* Copyright (C) Imrglop, All rights reserved
 * Copyright (C) Latite Client contributors, All rights reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited without the permission of the author.
 * Proprietary and confidential
 * Written by Imrglop <contact: latiteclientgithub@gmail.com>, 2023
 */

#pragma once
#include "api/feature/module/Module.h"
#include "api/eventing/Event.h"
#include "client/event/Eventing.h"

class Module : public IModule {
public:
	inline static constexpr int nokeybind = 1;

	explicit Module(std::string const& name, std::string const& displayName, std::string const& description, Category category,
		int keybind = 0, bool hud = false, bool visible = true) : IModule(name, description, displayName, category, visible) {
		this->hud = hud;
			{
				auto set = std::make_shared<Setting>("enabled", "Enabled", "Whether the module is on or not");
				set->value = &enabled;
				set->visible = false;

				settings->addSetting(set);
			}
			if (keybind != 1) {
				auto set = std::make_shared<Setting>("key", "Key", "The keybind of the module");
				set->value = &key;

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

	void loadConfig(SettingGroup& resolvedGroup) override;
	std::shared_ptr<Setting> addSetting(std::string const& internalName, std::string const& displayName, std::string const& desc, ValueType& val, Setting::Condition condition = Setting::Condition());
	std::shared_ptr<Setting> addEnumSetting(std::string const& internalName, std::string const& displayName, std::string const& desc, EnumData& dat, Setting::Condition condition = Setting::Condition());
	std::shared_ptr<Setting> addSliderSetting(std::string const& internalName, std::string const& displayName, std::string const& desc, ValueType& val, ValueType min, ValueType max, ValueType interval, Setting::Condition condition = Setting::Condition());
protected:
};