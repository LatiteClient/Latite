#pragma once
#include "api/eventing/Event.h"
#include "util/FNV32.h"
#include "util/DXUtil.h"

class RendererInitEvent : public Event {
public:
	static const uint32_t hash = TOHASH(RendererInitEvent);

	RendererInitEvent() {};
private:
};