#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinUser.h>

class Keyboard final {
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
private:
	void onChar(char ch, bool isChar = true);
};

