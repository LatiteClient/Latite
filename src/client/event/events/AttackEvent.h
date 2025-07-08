#pragma once
#include "client/event/Event.h"
#include "util/Crypto.h"

class AttackEvent : public Event {
public:
	static const uint32_t hash = TOHASH(AttackEvent);
private:
	SDK::Actor* entity;
public:

	SDK::Actor* getActor() {
		return entity;
	}

	AttackEvent(SDK::Actor* entity)
		: entity(entity)
	{
	}
};