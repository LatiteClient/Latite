#pragma once
#include "api/eventing/Event.h"
#include "util/FNV32.h"

class UpdateEvent : public Event {
public:
	static const uint32_t hash = TOHASH(UpdateEvent);

	UpdateEvent() {}
};