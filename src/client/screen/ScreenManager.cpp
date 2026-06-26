#include "pch.h"
#include "ScreenManager.h"
#include "screens/ClickGUI.h"
#include "screens/HUDEditor.h"
#include "mc/common/client/game/ClientInstance.h"
#include "client/event/events/KeyUpdateEvent.h"

ScreenManager::ScreenManager() {
    Eventing::get().listen<KeyUpdateEvent, &ScreenManager::onKey>(this);
    Eventing::get().listen<FocusLostEvent, &ScreenManager::onFocusLost>(this);
    Eventing::get().listen<UpdateEvent, &ScreenManager::onUpdate>(this);
}

void ScreenManager::activateScreen(Screen& screen, bool ignoreAnims) {
    if (this->activeScreen && &this->activeScreen->get() == &screen) {
        SDK::ClientInstance::get()->releaseCursor();
        return;
    }

    if (this->activeScreen) {
        this->activeScreen->get().setActive(false);
    }

    this->activeScreen = screen;
    screen.setActive(true, ignoreAnims);
    SDK::ClientInstance::get()->releaseCursor();
}

void ScreenManager::exitCurrentScreen() {
    if (this->activeScreen) {
        this->activeScreen->get().setActive(false);
        this->activeScreen = std::nullopt;
        SDK::ClientInstance::get()->grabCursor();
    }
}

void ScreenManager::onKey(KeyUpdateEvent& ev) {
    if (ev.isDown() && ev.getKey() == VK_ESCAPE && getActiveScreen()) {
        exitCurrentScreen();
        ev.setCancelled(true);
        return;
    }

    std::optional<std::reference_wrapper<Screen>> associatedScreen;
    this->forEach([&](Screen& s) {
        if (s.key == ev.getKey()) associatedScreen = s;
    });

    if (associatedScreen && ev.isDown() && (!ev.inUI() || getActiveScreen())) {
        if (getActiveScreen())
            exitCurrentScreen();
        else {
            activateScreen(associatedScreen->get());
        }
        ev.setCancelled(true);
        return;
    }
}

void ScreenManager::onFocusLost(FocusLostEvent& ev) {
    if (getActiveScreen()) {
        getActiveScreen()->get().resetInputState();
        if (auto client = SDK::ClientInstance::get()) {
            client->releaseCursor();
        }
        ev.setCancelled(true);
    }
}

void ScreenManager::onUpdate(UpdateEvent&) {
    auto client = SDK::ClientInstance::get();
    if (getActiveScreen() && client && client->minecraftGame && client->minecraftGame->isCursorGrabbed()) {
        client->releaseCursor();
    }
}
