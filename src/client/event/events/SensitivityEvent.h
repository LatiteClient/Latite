#pragma once
#include "client/event/Event.h"
#include "util/Crypto.h"

class SensitivityEvent : public Cancellable {
public:
	static const uint32_t hash = TOHASH(SensitivityEvent);

	[[nodiscard]] float& getValue() { return *this->value; }

	SensitivityEvent(float& value) : value(&value) {
	}

protected:
	float* value;
};
