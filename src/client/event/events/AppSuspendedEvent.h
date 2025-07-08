#pragma once
#include "client/event/Event.h"
#include "util/Crypto.h"

class AppSuspendedEvent : public Event {
public:
	static const uint32_t hash = TOHASH(AppSuspendedEvent);
protected:
};