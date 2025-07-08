#pragma once
#include "client/event/Event.h"
#include "util/Crypto.h"

class LeaveGameEvent : public Event {
public:
	LeaveGameEvent() = default;

	static const uint32_t hash = TOHASH(LeaveGameEvent);
};