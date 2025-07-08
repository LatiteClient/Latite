#pragma once
#include "client/event/Event.h"
#include "util/Crypto.h"

class GetTimeEvent : public Event {
public:
	static const uint32_t hash = TOHASH(GetTimeEvent);

	GetTimeEvent(float time) {
		this->time = time;
	}

	float getTime() {
		return time;
	}

	void setTime(float newTime) {
		time = newTime;
	}
protected:
	float time = 0.f;
};
