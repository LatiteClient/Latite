/* Copyright (C) Imrglop, All rights reserved
 * Copyright (C) Latite Client contributors, All rights reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited without the permission of the author.
 * Proprietary and confidential
 * Written by Imrglop <contact: latiteclientgithub@gmail.com>, 2023
 */

#pragma once
#include "api/eventing/Event.h"
#include "util/FNV32.h"

class CinematicCameraEvent : public Event {
public:
	static const uint32_t hash = TOHASH(CinematicCameraEvent);

	[[nodiscard]] void setValue(bool b) { this->b = b; }
	[[nodiscard]] bool getValue() { return b; }

	CinematicCameraEvent(bool b) : b(b) {
	}

protected:
	bool b;
};
