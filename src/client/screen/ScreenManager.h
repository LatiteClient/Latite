#pragma once
#include "api/manager/Manager.h"
#include "Screen.h"

class ScreenManager : public Manager<Screen> {
public:
	ScreenManager();

	bool showScreen(std::string const& screenName, bool ignoreAnims = false);
	bool tryToggleScreen(std::string const& screenName);
	void exitCurrentScreen();

	[[nodiscard]] std::shared_ptr<Screen> getActiveScreen() { return activeScreen; };
private:
	std::shared_ptr<Screen> activeScreen;
};