#pragma once
#include "api/feature/module/ModuleManager.h"
#include "api/eventing/Event.h"
#include "api/eventing/Listenable.h"
#include "Module.h"
#include "script/JsModule.h"

class ModuleManager : public Listener, public IModuleManager {
public:
	ModuleManager();
	~ModuleManager() = default;

	bool registerScriptModule(JsModule* mod) {
		for (auto& mod_ : items) {
			if (mod_->name() == mod->name()) {
				return false;
			}
		}
		this->items.push_back(std::shared_ptr<JsModule>(mod));
		return true;
	}

	bool deregisterScriptModule(JsModule* mod) {
		for (auto it = items.begin(); it != items.end(); it++) {
			if (it->get() == mod) {
				items.erase(it);
				return true;
			}
		}
		return false;
	}

	void onKey(Event& ev);
};