#pragma once
#include "client/event/Event.h"
#include "util/Crypto.h"
#include "util/DXUtil.h"

class RendererInitEvent : public Event {
public:
	static const uint32_t hash = TOHASH(RendererInitEvent);

	RendererInitEvent() {};
private:
};