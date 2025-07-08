#pragma once
#include "client/event/Event.h"
#include "util/Crypto.h"

class GammaEvent : public Cancellable {
public:
	static const uint32_t hash = TOHASH(GammaEvent);

	[[nodiscard]] float& getGamma() { return *gamma; }

	GammaEvent(float& gamma) : gamma(&gamma) {
	}

protected:
	float* gamma;
};
