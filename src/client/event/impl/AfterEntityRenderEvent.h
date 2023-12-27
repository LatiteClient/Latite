#pragma once
#include "api/eventing/Event.h"
#include "util/FNV32.h"
#include <sdk/common/world/actor/Actor.h>

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