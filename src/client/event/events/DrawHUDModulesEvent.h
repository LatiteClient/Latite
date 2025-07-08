#pragma once
#include "../Eventing.h"
#include "util/Crypto.h"

class DrawHUDModulesEvent : public Cancellable {
public:
	static const uint32_t hash = TOHASH(DrawHUDModulesEvent);
};