#pragma once
#include "screens/ClickGUI.h"
#include "screens/HUDEditor.h"

#include "client/manager/StaticManager.h"
#include "client/event/Listener.h"
#include "Screen.h"

#include "util/Util.h"
#include "script/JsScreen.h"

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

	bool registerScriptScreen(JsScreen* jsScn) {
		bool has = false;
		forEach([&](Screen& scn) {
			if (scn.getName() == jsScn->getName()) {
				has = true;
			}
			});

		if (has) return false;

		for (auto& screen : dynamicItems) {
			if (screen->getName() == jsScn->getName()) {
				return false;
			}
		}

		this->dynamicItems.push_back(std::shared_ptr<JsScreen>(jsScn));
		JS::JsAddRef(jsScn->getObject(), nullptr);
		return true;
	}

	bool deregisterScriptScreen(JsScreen* scn) {
		return std::erase_if(dynamicItems, [&](std::shared_ptr<Screen>& obj) {
			return obj->getName() == scn->getName();
			}) > 0;
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

	void onKey(KeyUpdateEvent& ev);
	void onFocusLost(FocusLostEvent& ev);
private:
	std::optional<std::reference_wrapper<Screen>> activeScreen;
};