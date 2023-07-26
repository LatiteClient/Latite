#pragma once
#include "api/feature/module/Module.h"
#include "api/eventing/Event.h"
#include "client/event/Eventing.h"

class Module : public IModule {
public:
	explicit Module(std::string const& name, std::string const& displayName, std::string const& description, Category category = GAME,
		bool visible = true) : IModule(name, description, displayName, category, visible) {}

	template <typename Event, typename Listener>
	void listen(Listener listener, bool callWhenInactive = false, int priority = 0) {
		Eventing::get().listen<Event>(this, (EventListenerFunc)listener, priority, callWhenInactive);
	}

	void loadConfig(SettingGroup& resolvedGroup) override;
	void addSetting(std::string const& internalName, std::string const& dipslayName, std::string const& desc, Setting::Value& val);
protected:
};