/* Copyright (C) Imrglop, All rights reserved
 * Copyright (C) Latite Client contributors, All rights reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited without the permission of the author.
 * Proprietary and confidential
 * Written by Imrglop <contact: latiteclientgithub@gmail.com>, 2023
 */

#pragma once
#include "api/eventing/Event.h"
#include "util/FNV32.h"

class AveragePingEvent : public Event {
public:
	static const uint32_t hash = TOHASH(AveragePingEvent);

	[[nodiscard]] int getPing() { return ping; }

	AveragePingEvent(int ping) : ping(ping) {
	}

protected:
	int ping;
};
