#include "pch.h"
#include "SpeedDisplay.h"
#include "client/event/Eventing.h"
#include "client/event/impl/TickEvent.h"

SpeedDisplay::SpeedDisplay() : TextModule("SpeedDisplay", "Speed Display", "Displays your speed in blocks per second.", HUD) {
	addSliderSetting("decimals", "Decimals", "The number of decimals to show", decimals, FloatValue(0.f), FloatValue(10.f), FloatValue(1.f));
	addSetting("includeY", "Y Motion", "Include Y motion in the speed value", includeY);

	this->suffix = TextValue(L" m/s");

	listen<TickEvent>((EventListenerFunc)&SpeedDisplay::onTick);
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
