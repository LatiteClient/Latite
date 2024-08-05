#include "pch.h"
#include "HurtColor.h"
#include "client/event/Eventing.h"
#include "client/event/impl/OverlayColorEvent.h"

HurtColor::HurtColor() : Module("HurtColor", "Hurt Color", "Change the color when entities get hit", IModule::GAME) {
	addSetting("color", "Color", "", color);

	listen<OverlayColorEvent>((EventListenerFunc)&HurtColor::onActorOverlay);
}

void HurtColor::onActorOverlay(Event& evGeneric) {
	auto& ev = reinterpret_cast<OverlayColorEvent&>(evGeneric);
	auto setColor = std::get<ColorValue>(this->color).getMainColor();
	if (ev.getActor() && ev.getActor()->invulnerableTime > 0) {
		ev.getColor() = { setColor.r, setColor.g, setColor.b, setColor.a };
	}
}
