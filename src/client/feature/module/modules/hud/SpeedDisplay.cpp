#include "pch.h"
#include "SpeedDisplay.h"
#include "client/event/Eventing.h"
#include "client/event/events/TickEvent.h"

SpeedDisplay::SpeedDisplay() : TextModule("SpeedDisplay", LocalizeString::get("client.textmodule.speedDisplay.name"),
                                          LocalizeString::get("client.textmodule.speedDisplay.desc"), HUD) {
    addSliderSetting("decimals", LocalizeString::get("client.textmodule.speedDisplay.decimals.name"),
                     LocalizeString::get("client.textmodule.speedDisplay.decimals.desc"), decimals, FloatValue(0.f),
                     FloatValue(10.f), FloatValue(1.f));
    addSetting("includeY", LocalizeString::get("client.textmodule.speedDisplay.includeY.name"),
               LocalizeString::get("client.textmodule.speedDisplay.includeY.desc"), includeY);

    this->suffix = TextValue(L" m/s");

    listen<TickEvent>(static_cast<EventListenerFunc>(&SpeedDisplay::onTick));
}

void SpeedDisplay::onTick(Event& evGeneric) {
	auto plr = SDK::ClientInstance::get()->getLocalPlayer();
	if (!plr) return;

	auto pos = plr->getPos();
	auto oldPos = plr->getPosOld();

	if (!std::get<BoolValue>(this->includeY)) {
		oldPos.y = pos.y;
	}

	speed = pos.distance(oldPos) * 20.f /*ticks per second*/;

}

std::wstringstream SpeedDisplay::text(bool isDefault, bool inEditor) {
	std::wstringstream wss;
	wss << std::fixed << std::setprecision(static_cast<std::streamsize>(std::get<FloatValue>(decimals))) << speed;
	return wss;
}
