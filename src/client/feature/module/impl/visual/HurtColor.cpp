/* Copyright (C) Imrglop, All rights reserved
 * Copyright (C) Latite Client contributors, All rights reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited without the permission of the author.
 * Proprietary and confidential
 * Written by Imrglop <contact: latiteclientgithub@gmail.com>, 2023
 */

#include "HurtColor.h"
#include "client/event/Eventing.h"
#include "client/event/impl/OverlayColorEvent.h"
#include "util/Util.h"

HurtColor::HurtColor() : Module("HurtColor", "Hurt Color", "Change the color when entities get hit", IModule::GAME) {
	addSetting("color", "Color", "", color);

	listen<OverlayColorEvent>((EventListenerFunc)&HurtColor::onActorOverlay);
}

void HurtColor::onActorOverlay(Event& evGeneric) {
	auto& ev = reinterpret_cast<OverlayColorEvent&>(evGeneric);
	auto setColor = std::get<ColorValue>(this->color).color1;
	if (ev.getActor() && ev.getActor()->invulnerableTime > 0) {
		ev.getColor() = { setColor.r, setColor.g, setColor.b, setColor.a };
	}
}
