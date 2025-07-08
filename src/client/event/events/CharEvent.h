#pragma once
#include "client/event/Event.h"
#include "util/Crypto.h"

class CharEvent : public Cancellable {
public:
	static const uint32_t hash = TOHASH(CharEvent);

	// Special Chars:
	// 0x1: Copy Request
	// 0x2: Enter Request
	// 0x3: Select All Request
	[[nodiscard]] wchar_t getChar() { return ch; }
	[[nodiscard]] bool isChar() { return isCharBool; }

	CharEvent(wchar_t ch, bool isChar = true) : ch(ch), isCharBool(isChar) {
	}

protected:
	wchar_t ch;
	bool isCharBool;
};
