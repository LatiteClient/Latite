#include "ScreenManager.h"
#include "impl/ClickGUI.h"
#include "util/Util.h"
#include "sdk/common/client/game/ClientInstance.h"

ScreenManager::ScreenManager() {
	this->mutex.lock();
	this->items.push_back(std::make_shared<ClickGUI>());
	this->mutex.unlock();
}

bool ScreenManager::showScreen(std::string const& screenName) {
	for (auto& screen : items) {
		if (util::toLower(screenName) == util::toLower(screen->getName())) {
			this->activeScreen = screen;
			this->activeScreen->setActive(true);
			return true;
		}
	}
	return false;
}

bool ScreenManager::tryToggleScreen(std::string const& screenName)
{
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
	this->activeScreen->setActive(false);
	this->activeScreen = nullptr;
	sdk::ClientInstance::get()->grabCursor();
}
