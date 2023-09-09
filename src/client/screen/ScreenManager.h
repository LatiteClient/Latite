#pragma once
#include "impl/ClickGUI.h"
#include "impl/HUDEditor.h"

#include "api/manager/StaticManager.h"
#include "api/eventing/Listenable.h"
#include "Screen.h"

#include "util/Util.h"

class ScreenManager : public Listener, public StaticManager<Screen,
	ClickGUI,
	HUDEditor> {
public:
	ScreenManager();

	template <typename T>
	void showScreen(bool ignoreAnims = false) {
		auto& scr = std::get<T>(items);

		this->activeScreen = scr;
		scr.setActive(true, ignoreAnims);
	}

	template <typename T>
	bool tryToggleScreen() {
		if (activeScreen) {
			showScreen<T>();
			return true;
		}
		if (activeScreen->get().getName() == std::get<T>(items).getName()) {
			this->exitCurrentScreen();
			return true;
		}
		return false;
	}
	void exitCurrentScreen();

	[[nodiscard]] std::optional<std::reference_wrapper<Screen>> getActiveScreen() { return activeScreen; };

	void onKey(Event& ev);
private:
	std::optional<std::reference_wrapper<Screen>> activeScreen;
};