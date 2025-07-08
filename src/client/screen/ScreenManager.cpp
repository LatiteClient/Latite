#include "pch.h"
#include "ScreenManager.h"
#include "screens/ClickGUI.h"
#include "screens/HUDEditor.h"
#include "mc/common/client/game/ClientInstance.h"
#include "client/event/events/KeyUpdateEvent.h"

ScreenManager::ScreenManager() {
	Eventing::get().listen<KeyUpdateEvent>(this, (EventListenerFunc)&ScreenManager::onKey);
}

void ScreenManager::exitCurrentScreen() {
	if (this->activeScreen) {
		this->activeScreen->get().setActive(false);
		this->activeScreen->get().onDisable();
		this->activeScreen = std::nullopt;
		SDK::ClientInstance::get()->grabCursor();
	}
}

void ScreenManager::onKey(Event& evGeneric) {
	static auto& ev = reinterpret_cast<KeyUpdateEvent&>(evGeneric);

	if (ev.isDown() && ev.getKey() == VK_ESCAPE && getActiveScreen()) {
		exitCurrentScreen();
		ev.setCancelled(true);
		return;
	}

	std::optional<std::reference_wrapper<Screen>> associatedScreen;
	this->forEach([&](Screen& s) {
		if (s.key == ev.getKey()) associatedScreen = s;
		});

	if (associatedScreen && ev.isDown()
		&& (!ev.inUI() || getActiveScreen())) {
		if (getActiveScreen())
			exitCurrentScreen();
		else {
			this->activeScreen = associatedScreen;
			associatedScreen->get().setActive(true);
			associatedScreen->get().onEnable(false);
		}
		ev.setCancelled(true);
		return;
	}
}