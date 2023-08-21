#pragma once
#include "api/eventing/Event.h"
#include "sdk/common/world/level/Level.h"
#include "util/FNV32.h"

class TickEvent : public Event {
public:
	static const uint32_t hash = TOHASH(TickEvent);
	
	[[nodiscard]] SDK::Level* getLevel() { return level; }

	TickEvent(SDK::Level* lvl) : level(lvl) {
	}

protected:
	SDK::Level* level;
};