#pragma once
#include "api/eventing/Event.h"
#include "util/chash.h"

class RenderGameEvent : public Event {
public:
	static const uint32_t hash = TOHASH(RenderGameEvent);

	RenderGameEvent() {}
};