#include "pch.h"
#include "BehindYou.h"
#include <client/input/Keyboard.h>
#include <client/Latite.h>
#include "client/localization/LocalizeString.h"

BehindYou::BehindYou() : Module("BehindYou", LocalizeString::get("client.module.behindYou.name"), LocalizeString::get("client.module.behindYou.desc"), GAME, nokeybind) {
	pers.addEntry(EnumEntry{ 0, LocalizeString::get("client.module.behindYouState0.name") });
	pers.addEntry(EnumEntry{ 1, LocalizeString::get("client.module.behindYouState1.name") });
	pers.addEntry(EnumEntry{ 2, LocalizeString::get("client.module.behindYouState2.name") });
	addEnumSetting("Perspective", LocalizeString::get("client.module.behindYou.perspective.name"), LocalizeString::get("client.module.behindYou.perspective.desc"), this->pers);
	addSetting("bind", LocalizeString::get("client.module.behindYou.bind.name"), LocalizeString::get("client.module.behindYou.bind.desc"), bind);

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
