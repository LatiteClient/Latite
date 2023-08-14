#pragma once
#include "api/eventing/Event.h"
#include "util/FNV32.h"
#include "util/Util.h"
#include "sdk/common/world/actor/Actor.h"

// FIXME: is this really cancellable?
class OverlayColorEvent : public Cancellable {
public:
	static const uint32_t hash = TOHASH(OverlayColorEvent);

	[[nodiscard]] Color& getColor() { return *color; }
	[[nodiscard]] sdk::Actor* getActor() { return actor; }

	OverlayColorEvent(Color& color, sdk::Actor* actor) : color(&color), actor(actor) {
	}

protected:
	Color* color;
	sdk::Actor* actor;
};
