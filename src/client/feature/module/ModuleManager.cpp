#include "ModuleManager.h"
#include "impl/misc/TestModule.h"
#include "impl/misc/Screenshot.h"
#include "impl/game/Zoom.h"
#include "impl/visual/Fullbright.h"
#include "impl/visual/MotionBlur.h"
#include "impl/visual/HurtColor.h"

#include "impl/hud/FPSCounter.h"
#include "impl/hud/CPSCounter.h"

#include "client/event/impl/KeyUpdateEvent.h"

// TODO: queue shutdown and disalbe all modules (after config saves)
ModuleManager::ModuleManager() {
#ifdef LATITE_DEBUG
	this->items.push_back(std::make_shared<TestModule>());
#endif
	this->items.push_back(std::make_shared<Zoom>());
	this->items.push_back(std::make_shared<Fullbright>());
	this->items.push_back(std::make_shared<MotionBlur>());
	this->items.push_back(std::make_shared<HurtColor>());
	//this->items.push_back(std::make_shared<Screenshot>());
	this->items.push_back(std::make_shared<FPSCounter>());
	this->items.push_back(std::make_shared<CPSCounter>());

	for (auto& mod : items) {
		mod->onInit();
	}
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
