#pragma once
#include "api/feature/module/ModuleManager.h"
#include "api/eventing/Event.h"
#include "api/eventing/Listenable.h"

class ModuleManager : public Listener, public IModuleManager {
public:
	ModuleManager();
	~ModuleManager() = default;

	void onKey(Event& ev);
};