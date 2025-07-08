#pragma once
#include "client/event/Event.h"
#include "util/Crypto.h"
#include <mc/common/world/actor/Actor.h>

// FIXME: AfterRenderEntityEvent does not trigger after 1.21.92 due to actor rendering changes or some shit
class AfterRenderEntityEvent : public Event {
public:
	static const auto hash = TOHASH(AfterRenderEntityEvent);

	AfterRenderEntityEvent(SDK::Actor* entity, Vec3& position) : entity(entity), position(&position) {};

	[[nodiscard]] SDK::Actor* getEntity() { return entity; }
	[[nodiscard]] Vec3& getPosition() { return *position; }

private:
	SDK::Actor* entity;
	Vec3* position;
};