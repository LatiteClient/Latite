#pragma once
#include "api/eventing/Event.h"
#include "util/FNV32.h"
#include "sdk/common/entity/component/MoveInputComponent.h"

class BeforeMoveEvent : public Cancellable {
public:
	static const uint32_t hash = TOHASH(BeforeMoveEvent);

	[[nodiscard]] SDK::MoveInputComponent* getMoveInputHandler() { return handler; }

	BeforeMoveEvent(SDK::MoveInputComponent* handler) : handler(handler) {
	}

protected:
	SDK::MoveInputComponent* handler;
};
