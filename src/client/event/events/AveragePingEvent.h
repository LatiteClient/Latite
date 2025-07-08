#pragma once
#include "client/event/Event.h"
#include "util/Crypto.h"

class AveragePingEvent : public Event {
public:
	static const uint32_t hash = TOHASH(AveragePingEvent);

	[[nodiscard]] int getPing() { return ping; }

	AveragePingEvent(int ping) : ping(ping) {
	}

protected:
	int ping;
};
