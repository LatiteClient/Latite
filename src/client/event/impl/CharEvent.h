#pragma once
#include "api/eventing/Event.h"
#include "util/chash.h"

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
