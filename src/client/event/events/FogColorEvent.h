#pragma once
#include "client/event/Event.h"
#include "util/Crypto.h"

class FogColorEvent : public Event {
public:
	static const uint32_t hash = TOHASH(FogColorEvent);

	FogColorEvent(Color* color) : color(color) {}

	void setColor(float r, float g, float b) {
		color->r = r;
		color->g = g;
		color->b = b;
	}

protected:
	Color* color;
};
