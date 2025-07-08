#pragma once
#include "client/event/Event.h"
#include "util/Crypto.h"

class FocusLostEvent : public Cancellable {
public:
	static const uint32_t hash = TOHASH(FocusLostEvent);
	FocusLostEvent() = default;
private:
};