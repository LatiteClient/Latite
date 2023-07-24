#pragma once
#include "api/eventing/Event.h"
#include "util/chash.h"

class FocusLostEvent : public Cancellable {
public:
	static const uint32_t hash = TOHASH(FocusLostEvent);
	FocusLostEvent() = default;
private:
};