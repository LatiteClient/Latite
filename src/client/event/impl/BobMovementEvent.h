#pragma once
#include "api/eventing/Event.h"
#include "util/FNV32.h"

class BobMovementEvent : public Cancellable {
public:
	BobMovementEvent() = default;

	static const uint32_t hash = TOHASH(BobMovementEvent);
};