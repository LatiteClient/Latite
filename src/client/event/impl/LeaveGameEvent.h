#pragma once
#include "api/eventing/Event.h"
#include "util/FNV32.h"

class LeaveGameEvent : public Event {
public:
	LeaveGameEvent() = default;

	static const uint32_t hash = TOHASH(LeaveGameEvent);
};