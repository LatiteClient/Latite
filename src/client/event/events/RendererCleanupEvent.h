#pragma once
#include "client/event/Event.h"
#include "util/Crypto.h"
#include "util/DXUtil.h"

class RendererCleanupEvent : public Event {
public:
	static const uint32_t hash = TOHASH(RendererCleanupEvent);

	RendererCleanupEvent() {};
private:
};