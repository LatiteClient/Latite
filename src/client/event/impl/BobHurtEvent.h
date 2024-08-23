#pragma once
#include "api/eventing/Event.h"
#include "util/FNV32.h"

class BobHurtEvent : public Cancellable {
public:
	static const uint32_t hash = TOHASH(BobHurtEvent);
	BobHurtEvent() {}
};