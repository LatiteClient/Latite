#include "pch.h"
#include "Fullbright.h"

Fullbright::Fullbright() : Module("Fullbright", LocalizeString::get("client.module.fullbright.name"),
                                  LocalizeString::get("client.module.fullbright.desc"), GAME) {
    addSliderSetting("gamma", LocalizeString::get("client.module.fullbright.gamma.name"),
                     LocalizeString::get("client.module.fullbright.gamma.desc"), this->gamma, FloatValue(0.f),
                     FloatValue(25.f), FloatValue(1.f));
    listen<GammaEvent>(&Fullbright::onGamma);
}

void Fullbright::onGamma(Event& genericEv) {
	auto& ev = reinterpret_cast<GammaEvent&>(genericEv);
	ev.getGamma() = std::get<FloatValue>(this->gamma);
}
