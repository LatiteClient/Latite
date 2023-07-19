#pragma once
#include "api/feature/module/Module.h"
#include "api/eventing/Event.h"
#include "client/event/Eventing.h"

class Module : public IModule {
public:
	explicit Module(std::string const& name, std::string const& description,
		std::string const& displayName) : IModule(name, description, displayName) {}

	template <typename Event, typename Listener>
	void listen(Listener listener) {
		Eventing::get().listen<Event>(this, (EventListenerFunc)listener);
	}

	void loadConfig(SettingGroup& resolvedGroup) override;
};