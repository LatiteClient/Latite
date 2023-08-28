#include "pch.h"
#include "ModuleManager.h"
#include "impl/misc/TestModule.h"
#include "impl/misc/Screenshot.h"
#include "impl/misc/Clock.h"
#include "impl/misc/DebugInfo.h"

#include "impl/game/Zoom.h"
#include "impl/game/ToggleSprintSneak.h"

#include "impl/visual/Fullbright.h"
#include "impl/visual/MotionBlur.h"
#include "impl/visual/HurtColor.h"

#include "impl/hud/FPSCounter.h"
#include "impl/hud/CPSCounter.h"
#include "impl/hud/ServerDisplay.h"
#include "impl/hud/PingDisplay.h"
#include "impl/hud/SpeedDisplay.h"
#include "impl/hud/BowIndicator.h"
#include "impl/hud/GuiscaleChanger.h"

#include "client/event/impl/KeyUpdateEvent.h"

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
	this->items.push_back(std::make_shared<ServerDisplay>());
	this->items.push_back(std::make_shared<PingDisplay>());
	this->items.push_back(std::make_shared<SpeedDisplay>());
	this->items.push_back(std::make_shared<ToggleSprintSneak>());
	this->items.push_back(std::make_shared<Clock>());
	this->items.push_back(std::make_shared<BowIndicator>());
	this->items.push_back(std::make_shared<GuiscaleChanger>());
#if LATITE_DEBUG
	this->items.push_back(std::make_shared<Screenshot>());
#endif
	this->items.push_back(std::make_shared<DebugInfo>());

	for (auto& mod : items) {
		mod->onInit();
	}
	Eventing::get().listen<KeyUpdateEvent>(this, (EventListenerFunc) & ModuleManager::onKey);
}

ModuleManager::~ModuleManager() {
	for (auto& mod : items) {
		if (mod->isEnabled()) mod->setEnabled(false);
	}
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
