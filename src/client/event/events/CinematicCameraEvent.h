#pragma once
#include "client/event/Event.h"
#include "util/Crypto.h"

class CinematicCameraEvent : public Event {
public:
	static const uint32_t hash = TOHASH(CinematicCameraEvent);

	[[nodiscard]] void setValue(bool b) { this->b = b; }
	[[nodiscard]] bool getValue() { return b; }

	CinematicCameraEvent(bool b) : b(b) {
	}

protected:
	bool b;
};
