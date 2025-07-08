#pragma once
#include "client/event/Event.h"
#include "util/Crypto.h"

class BobMovementEvent : public Cancellable {
public:
	BobMovementEvent() = default;

	static const uint32_t hash = TOHASH(BobMovementEvent);
};