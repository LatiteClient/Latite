#pragma once
#include "client/event/Event.h"
#include "util/Crypto.h"
#include "mc/common/entity/component/MoveInputComponent.h"

class AfterMoveEvent : public Event {
public:
	static const uint32_t hash = TOHASH(AfterMoveEvent);

	[[nodiscard]] SDK::MoveInputComponent* getMoveInputHandler() { return handler; }

	AfterMoveEvent(SDK::MoveInputComponent* handler) : handler(handler) {
	}

protected:
	SDK::MoveInputComponent* handler;
};
