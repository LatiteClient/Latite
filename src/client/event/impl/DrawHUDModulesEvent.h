#pragma once
#include "../Eventing.h"
#include "util/FNV32.h"

class DrawHUDModulesEvent : public Cancellable {
public:
	static const uint32_t hash = TOHASH(DrawHUDModulesEvent);
};