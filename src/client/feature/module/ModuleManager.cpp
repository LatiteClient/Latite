#include "pch.h"
#include "ModuleManager.h"
#include "modules/misc/TestModule.h"
#include "modules/misc/DebugInfo.h"
#include "modules/misc/Nickname.h"
#include "modules/misc/ItemTweaks.h"
#include "modules/misc/DebugInfo.h"
#include "modules/misc/CommandShortcuts.h"
#include "modules/misc/BlockGame.h"

#include "modules/game/Zoom.h"
#include "modules/game/CinematicCamera.h"
#include "modules/game/ToggleSprintSneak.h"
#include "modules/game/BehindYou.h"
#include "modules/game/ThirdPersonNametag.h"
#include "modules/game/EnvironmentChanger.h"
#include "modules/game/TextHotkey.h"
#include "modules/game/Freelook.h"
#include "modules/game/AutoGG.h"

#include "modules/visual/Fullbright.h"
#include "modules/visual/MotionBlur.h"
#include "modules/visual/HurtColor.h"
#include "modules/visual/Hitboxes.h"
#include "modules/visual/ChunkBorders.h"
#include "modules/visual/Hitboxes.h"
#include "modules/visual/BlockOutline.h"

#include "modules/hud/FPSCounter.h"
#include "modules/hud/CPSCounter.h"
#include "modules/hud/ServerDisplay.h"
#include "modules/hud/PingDisplay.h"
#include "modules/hud/SpeedDisplay.h"
#include "modules/hud/Clock.h"
#include "modules/hud/BowIndicator.h"
#include "modules/hud/GuiscaleChanger.h"
#include "modules/hud/TabList.h"
#include "modules/hud/Keystrokes.h"
#include "modules/hud/BreakIndicator.h"
#include "modules/hud/HealthWarning.h"
#include "modules/hud/ArmorHUD.h"
#include "modules/hud/MovablePaperdoll.h"
#include "modules/hud/MovableScoreboard.h"
#include "modules/hud/ReachDisplay.h"
#include "modules/hud/MovableBossbar.h"
#include "modules/hud/ItemCounter.h"
#include "modules/hud/Chat.h"
#include "modules/hud/ComboCounter.h"
#include "modules/hud/CustomCoordinates.h"
#include "modules/hud/MovableCoordinates.h"
#include "modules/hud/FrameTimeDisplay.h"

#include "client/event/events/KeyUpdateEvent.h"

ModuleManager::ModuleManager() {
#ifdef LATITE_DEBUG
	this->items.push_back(std::make_shared<TestModule>());
#endif
	this->items.push_back(std::make_shared<Zoom>());
	this->items.push_back(std::make_shared<Fullbright>());
	this->items.push_back(std::make_shared<MotionBlur>());
	//this->items.push_back(std::make_shared<HurtColor>());
	this->items.push_back(std::make_shared<FPSCounter>());
	this->items.push_back(std::make_shared<CPSCounter>());
	this->items.push_back(std::make_shared<ServerDisplay>());
	this->items.push_back(std::make_shared<PingDisplay>());
	this->items.push_back(std::make_shared<SpeedDisplay>());
	this->items.push_back(std::make_shared<ToggleSprintSneak>());
	this->items.push_back(std::make_shared<Clock>());
	this->items.push_back(std::make_shared<BowIndicator>());
	this->items.push_back(std::make_shared<GuiscaleChanger>());
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
	//this->items.push_back(std::make_shared<Hitboxes>());
	this->items.push_back(std::make_shared<BlockOutline>());
	this->items.push_back(std::make_shared<MovableScoreboard>());
	this->items.push_back(std::make_shared<Nickname>());
	this->items.push_back(std::make_shared<ReachDisplay>());
	this->items.push_back(std::make_shared<MovableBossbar>());
	this->items.push_back(std::make_shared<ThirdPersonNametag>());
	this->items.push_back(std::make_shared<EnvironmentChanger>());
	this->items.push_back(std::make_shared<CommandShortcuts>());
	this->items.push_back(std::make_shared<ItemCounter>());
	//this->items.push_back(std::make_shared<Chat>());
	this->items.push_back(std::make_shared<TextHotkey>());
	this->items.push_back(std::make_shared<Freelook>());
	this->items.push_back(std::make_shared<ComboCounter>());
	this->items.push_back(std::make_shared<CustomCoordinates>());
	this->items.push_back(std::make_shared<MovableCoordinates>());
	this->items.push_back(std::make_shared<AutoGG>());
	this->items.push_back(std::make_shared<FrameTimeDisplay>());
	this->items.push_back(std::make_shared<BlockGame>());

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
