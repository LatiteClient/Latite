#include "pch.h"
#include "BehindYou.h"
#include <client/input/Keyboard.h>
#include <client/Latite.h>
#include "client/localization/LocalizeString.h"

BehindYou::BehindYou() : Module("BehindYou", LocalizeString::get("client.module.behindyou.name"), LocalizeString::get("client.module.behindyou.desc"), GAME, nokeybind) {
	pers.addEntry(EnumEntry{ 0, "Third Person Front" });
	pers.addEntry(EnumEntry{ 1, "Third Person Back" });
	pers.addEntry(EnumEntry{ 2, "First Person" });
	addEnumSetting("Perspective", "Perspective", "The perspective", this->pers);
	addSetting("bind", "Bind", "What to bind the perspective to", bind);

	listen<PerspectiveEvent>((EventListenerFunc)&BehindYou::onPerspective);
}

void BehindYou::onPerspective(Event& evG) {
	auto& ev = reinterpret_cast<PerspectiveEvent&>(evG);
	if (SDK::ClientInstance::get()->minecraftGame->isCursorGrabbed()) {
		if (Latite::getKeyboard().isKeyDown(std::get<KeyValue>(this->bind))) {
			ev.getView() = 2 - this->pers.getSelectedKey(); // convert to actual mc perspective
		}
	}
}
