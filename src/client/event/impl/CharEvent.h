/* Copyright (C) Imrglop, All rights reserved
 * Copyright (C) Latite Client contributors, All rights reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited without the permission of the author.
 * Proprietary and confidential
 * Written by Imrglop <contact: latiteclientgithub@gmail.com>, 2023
 */

#pragma once
#include "api/eventing/Event.h"
#include "util/FNV32.h"

class CharEvent : public Cancellable {
public:
	static const uint32_t hash = TOHASH(CharEvent);

	// Special Chars:
	// 0x1: Copy Request
	// 0x2: Enter Request
	// 0x3: Select All Request
	[[nodiscard]] char getChar() { return ch; }
	[[nodiscard]] bool isChar() { return isCharBool; }

	CharEvent(char ch, bool isChar = true) : ch(ch), isCharBool(isChar) {
	}

protected:
	char ch;
	bool isCharBool;
};
