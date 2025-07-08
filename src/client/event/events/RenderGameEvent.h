#pragma once
#include "client/event/Event.h"
#include "util/Crypto.h"

class RenderGameEvent : public Event {
public:
	static const uint32_t hash = TOHASH(RenderGameEvent);

	RenderGameEvent() {}
};