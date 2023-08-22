/* Copyright (C) Imrglop, All rights reserved
 * Copyright (C) Latite Client contributors, All rights reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited without the permission of the author.
 * Proprietary and confidential
 * Written by Imrglop <contact: latiteclientgithub@gmail.com>, 2023
 */

#pragma once
#include "api/eventing/Event.h"
#include "sdk/common/world/level/Level.h"
#include "util/FNV32.h"

class TickEvent : public Event {
public:
	static const uint32_t hash = TOHASH(TickEvent);
	
	[[nodiscard]] SDK::Level* getLevel() { return level; }

	TickEvent(SDK::Level* lvl) : level(lvl) {
	}

protected:
	SDK::Level* level;
};