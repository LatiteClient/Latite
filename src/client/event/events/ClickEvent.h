#pragma once
#include "client/event/Event.h"
#include "util/Crypto.h"

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