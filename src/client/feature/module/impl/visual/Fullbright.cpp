/* Copyright (C) Imrglop, All rights reserved
 * Copyright (C) Latite Client contributors, All rights reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited without the permission of the author.
 * Proprietary and confidential
 * Written by Imrglop <contact: latiteclientgithub@gmail.com>, 2023
 */

#include "Fullbright.h"

Fullbright::Fullbright() : Module("Fullbright", "Fullbright", "Extra world brightness", GAME) {
	addSliderSetting("gamma", "Gamma", "The amount of light", this->gamma, FloatValue(0.f), FloatValue(25.f), FloatValue(1.f));
	listen<GammaEvent>(&Fullbright::onGamma);
}

void Fullbright::onGamma(Event& genericEv) {
	auto& ev = reinterpret_cast<GammaEvent&>(genericEv);
	ev.getGamma() = std::get<FloatValue>(this->gamma);
}
