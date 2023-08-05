#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinUser.h>

#include "client/event/Eventing.h"
#include "client/event/impl/KeyUpdateEvent.h"

class Keyboard final : public Listener {
public:
	Keyboard(int* gameKeyMap);
	Keyboard(Keyboard&) = delete;
	Keyboard(Keyboard&&) = delete;

	~Keyboard() = default;

	int* keyMap;
	int keyMapAdjusted[0x100] = {};
	int keyMapOld[0x100] = {};
	BYTE winKeyMap[0x100] = {};

	void findTextInput();
	bool isKeyDown(int vKey);

	void onKey(Event& ev);
private:
	void onChar(char ch, bool isChar = true);
};

