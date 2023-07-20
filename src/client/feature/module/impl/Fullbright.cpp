#include "Fullbright.h"

Fullbright::Fullbright() : Module("Fullbright", "Fullbright", "Lights up your world") {
	addSetting("gamma", "Gamma", "The amount of light", this->gamma);
	listen<GammaEvent>(&Fullbright::onGamma);
}

void Fullbright::onGamma(Event& genericEv) {
	auto ev = reinterpret_cast<GammaEvent&>(genericEv);
	ev.getGamma() = std::get<FloatValue>(this->gamma);
}
