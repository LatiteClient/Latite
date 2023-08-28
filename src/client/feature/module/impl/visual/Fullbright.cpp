#include "Fullbright.h"
#include "pch.h"

Fullbright::Fullbright() : Module("Fullbright", "Fullbright", "Extra world brightness", GAME) {
	addSliderSetting("gamma", "Gamma", "The amount of light", this->gamma, FloatValue(0.f), FloatValue(25.f), FloatValue(1.f));
	listen<GammaEvent>(&Fullbright::onGamma);
}

void Fullbright::onGamma(Event& genericEv) {
	auto& ev = reinterpret_cast<GammaEvent&>(genericEv);
	ev.getGamma() = std::get<FloatValue>(this->gamma);
}
