#pragma once
#include "api/eventing/Event.h"
#include "util/FNV32.h"

class HideHandEvent : public Cancellable {
public:
	static const uint32_t hash = TOHASH(HideHandEvent);

	[[nodiscard]] bool& getValue() { return *this->value; }

	HideHandEvent(bool& value) : value(&value) {
	}

protected:
	bool* value;
};
