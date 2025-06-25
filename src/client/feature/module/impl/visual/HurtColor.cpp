#include "pch.h"
#include "HurtColor.h"
#include "client/event/Eventing.h"
#include "client/event/impl/OverlayColorEvent.h"

HurtColor::HurtColor() : Module("HurtColor", LocalizeString::get("client.module.hurtColor.name"),
                                LocalizeString::get("client.module.hurtColor.desc"),
                                GAME) {
    addSetting("color", LocalizeString::get("client.module.hurtColor.color.name"), L"", color);

    listen<OverlayColorEvent>(static_cast<EventListenerFunc>(&HurtColor::onActorOverlay));
}

// FIXME: either get a new function to hook if known in the future or find other way to get the actor color propert
void HurtColor::onActorOverlay(Event& evGeneric) {
	auto& ev = reinterpret_cast<OverlayColorEvent&>(evGeneric);
	auto setColor = std::get<ColorValue>(this->color).getMainColor();
	if (ev.getActor() && ev.getActor()->invulnerableTime > 0) {
		ev.getColor() = { setColor.r, setColor.g, setColor.b, setColor.a };
	}
}
