#pragma once
#include "client/event/Event.h"
#include "util/Crypto.h"
#include "util/DXUtil.h"

class RenderOverlayEvent : public Event {
public:
	static const uint32_t hash = TOHASH(RenderOverlayEvent);

	RenderOverlayEvent(ID2D1DeviceContext* ctx) : ctx(ctx) {};
	[[nodiscard]] ID2D1DeviceContext* getDeviceContext() noexcept { return ctx; };
private:
	ID2D1DeviceContext* ctx;
};