#include "pch.h"
#include "ModuleManager.h"
#include "impl/misc/TestModule.h"
#include "impl/misc/Screenshot.h"
#include "impl/misc/DebugInfo.h"
#include "impl/misc/Nickname.h"
#include "impl/misc/ItemTweaks.h"
#include "impl/misc/DebugInfo.h"

#include "impl/game/Zoom.h"
#include "impl/game/CinematicCamera.h"
#include "impl/game/ToggleSprintSneak.h"
#include "impl/game/BehindYou.h"
#include "impl/game/ThirdPersonNametag.h"

#include "impl/visual/Fullbright.h"
#include "impl/visual/MotionBlur.h"
#include "impl/visual/HurtColor.h"
#include "impl/visual/Hitboxes.h"
#include "impl/visual/ChunkBorders.h"
#include "impl/visual/Hitboxes.h"
#include "impl/visual/BlockOutline.h"

#include "impl/hud/FPSCounter.h"
#include "impl/hud/CPSCounter.h"
#include "impl/hud/ServerDisplay.h"
#include "impl/hud/PingDisplay.h"
#include "impl/hud/SpeedDisplay.h"
#include "impl/hud/Clock.h"
#include "impl/hud/BowIndicator.h"
#include "impl/hud/GuiscaleChanger.h"
#include "impl/hud/TabList.h"
#include "impl/hud/Keystrokes.h"
#include "impl/hud/BreakIndicator.h"
#include "impl/hud/HealthWarning.h"
#include "impl/hud/ArmorHUD.h"
#include "impl/hud/MovablePaperdoll.h"
#include "impl/hud/MovableScoreboard.h"
#include "impl/hud/ReachDisplay.h"
#include "impl/hud/MovableBossbar.h"

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
	this->items.push_back(std::make_shared<Screenshot>());
	this->items.push_back(std::make_shared<DebugInfo>());
	this->items.push_back(std::make_shared<TabList>());
	this->items.push_back(std::make_shared<Keystrokes>());
	this->items.push_back(std::make_shared<CinematicCamera>());
	this->items.push_back(std::make_shared<HealthWarning>());
	this->items.push_back(std::make_shared<BreakIndicator>());
	this->items.push_back(std::make_shared<BehindYou>());
	this->items.push_back(std::make_shared<ChunkBorders>());
	this->items.push_back(std::make_shared<ArmorHUD>());
	this->items.push_back(std::make_shared<MovablePaperdoll>());
	this->items.push_back(std::make_shared<Hitboxes>());
	this->items.push_back(std::make_shared<BlockOutline>());
	this->items.push_back(std::make_shared<MovableScoreboard>());
	this->items.push_back(std::make_shared<Nickname>());
	this->items.push_back(std::make_shared<ItemTweaks>());
	//this->items.push_back(std::make_shared<ReachDisplay>());
	this->items.push_back(std::make_shared<MovableBossbar>());
	this->items.push_back(std::make_shared<ThirdPersonNametag>());

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
		if (mod->getKeybind() == ev.getKey()) {
			if (mod->shouldHoldToToggle()) {
				if (!mod->isEnabled() && ev.isDown()) {
					mod->setEnabled(true);
				}
				else if (mod->isEnabled() && !ev.isDown()) {
					mod->setEnabled(false);
				}
				continue;
			}
			else if (ev.isDown()) {
				mod->setEnabled(!mod->isEnabled());
			}
		}
	}
}
