#pragma once
#include "client/event/Event.h"
#include "util/Crypto.h"

class BobHurtEvent : public Cancellable {
public:
	static const uint32_t hash = TOHASH(BobHurtEvent);
	BobHurtEvent() {}
};