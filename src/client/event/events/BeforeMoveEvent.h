#pragma once
#include "client/event/Event.h"
#include "util/Crypto.h"
#include "mc/common/entity/component/MoveInputComponent.h"

class BeforeMoveEvent : public Cancellable {
public:
	static const uint32_t hash = TOHASH(BeforeMoveEvent);

	[[nodiscard]] SDK::MoveInputComponent* getMoveInputHandler() { return handler; }

	BeforeMoveEvent(SDK::MoveInputComponent* handler) : handler(handler) {
	}

protected:
	SDK::MoveInputComponent* handler;
};
