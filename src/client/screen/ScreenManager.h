/* Copyright (C) Imrglop, All rights reserved
 * Copyright (C) Latite Client contributors, All rights reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited without the permission of the author.
 * Proprietary and confidential
 * Written by Imrglop <contact: latiteclientgithub@gmail.com>, 2023
 */

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
		scr.setActive(true);
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