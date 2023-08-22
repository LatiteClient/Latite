/* Copyright (C) Imrglop, All rights reserved
 * Copyright (C) Latite Client contributors, All rights reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited without the permission of the author.
 * Proprietary and confidential
 * Written by Imrglop <contact: latiteclientgithub@gmail.com>, 2023
 */

#pragma once
#include "api/eventing/Event.h"
#include "util/FNV32.h"

class ClickEvent : public Cancellable {
public:
	static const uint32_t hash = TOHASH(ClickEvent);

	[[nodiscard]] int getMouseButton() { return btn; }
	[[nodiscard]] int isDown() { return downOrDelta; }
	[[nodiscard]] char getWheelDelta() { return downOrDelta; }

	ClickEvent(int btn, char dod) : btn(btn), downOrDelta(dod) {}
private:
	int btn;
	char downOrDelta;
};