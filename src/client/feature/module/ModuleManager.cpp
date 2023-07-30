#include "ModuleManager.h"
#include "impl/TestModule.h"
#include "impl/Zoom.h"
#include "impl/Fullbright.h"
#include "impl/MotionBlur.h"

#include "client/event/impl/KeyUpdateEvent.h"

ModuleManager::ModuleManager() {
#ifdef LATITE_DEBUG
	this->items.push_back(std::make_shared<TestModule>());
#endif
	this->items.push_back(std::make_shared<Zoom>());
	this->items.push_back(std::make_shared<Fullbright>());
	this->items.push_back(std::make_shared<MotionBlur>());

	Eventing::get().listen<KeyUpdateEvent>(this, (EventListenerFunc) & ModuleManager::onKey);
}

void ModuleManager::onKey(Event& evGeneric) {
	auto& ev = reinterpret_cast<KeyUpdateEvent&>(evGeneric);
	for (auto& mod : items) {
		if (ev.inUI()) return;
		if (ev.isDown() && mod->getKeybind() == ev.getKey()) {
			mod->setEnabled(!mod->isEnabled());
		}
	}
}
