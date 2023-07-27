#include "ScreenManager.h"
#include "impl/ClickGUI.h"
#include "impl/HUDEditor.h"
#include "util/Util.h"
#include "sdk/common/client/game/ClientInstance.h"

ScreenManager::ScreenManager() {
	this->mutex.lock();
	this->items.push_back(std::make_shared<ClickGUI>());
	this->items.push_back(std::make_shared<HUDEditor>());
	this->mutex.unlock();

}

bool ScreenManager::showScreen(std::string const& screenName, bool ignoreAnims) {
	for (auto& screen : items) {
		if (util::ToLower(screenName) == util::ToLower(screen->getName())) {
			this->activeScreen = screen;
			this->activeScreen->setActive(true);
			this->activeScreen->onEnable(ignoreAnims);
			return true;
		}
	}
	return false;
}

bool ScreenManager::tryToggleScreen(std::string const& screenName) {
	if (!this->activeScreen) {
		return showScreen(screenName);
	}
	else {
		if (activeScreen->getName() == screenName) {
			this->exitCurrentScreen();
			return true;
		}
	}
	return false;
}

void ScreenManager::exitCurrentScreen() {
	if (this->activeScreen) {
		this->activeScreen->setActive(false);
		this->activeScreen->onDisable();
		this->activeScreen = nullptr;
		sdk::ClientInstance::get()->grabCursor();
	}
}
