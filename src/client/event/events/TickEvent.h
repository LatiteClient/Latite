#pragma once
#include "client/event/Event.h"
#include "mc/common/world/level/Level.h"
#include "util/Crypto.h"

class TickEvent : public Event {
public:
	static const uint32_t hash = TOHASH(TickEvent);
	
	[[nodiscard]] SDK::Level* getLevel() { return level; }

	TickEvent(SDK::Level* lvl) : level(lvl) {
	}

protected:
	SDK::Level* level;
};