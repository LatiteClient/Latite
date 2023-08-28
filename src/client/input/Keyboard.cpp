#include "Keyboard.h"
#include "client/event/impl/CharEvent.h"
#include "sdk/common/client/input/ClientInputHandler.h"
#include <chrono>
#include "pch.h"

Keyboard::Keyboard(int* gameKeyMap) : keyMap(gameKeyMap) {
	Eventing::get().listen<KeyUpdateEvent>(this, (EventListenerFunc)&Keyboard::onKey, 4);
}

void Keyboard::findTextInput() {
	bool backspaceHeld = keyMapAdjusted['\b'];
	static std::chrono::high_resolution_clock::time_point backspaceStart = std::chrono::high_resolution_clock::now();
	if (!backspaceHeld) {
		backspaceStart = std::chrono::high_resolution_clock::now();
	}
	auto timeNow = std::chrono::high_resolution_clock::now();

	auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(timeNow - backspaceStart).count();

	for (int i = 0; i < 256; i++) {
		winKeyMap[i] = static_cast<BYTE>(keyMapAdjusted[i]);
		if (i == VK_SHIFT) {
			winKeyMap[i] = keyMapAdjusted[i] ? 0x80 : 0x0;
		}
		int caps = GetKeyState(VK_CAPITAL);
		winKeyMap[VK_CAPITAL] = caps;

		bool isDown = keyMapAdjusted[i];
		bool oldIsDown = keyMapOld[i];

		if (isDown && !oldIsDown && i != VK_ESCAPE) {

			if (keyMapAdjusted[VK_RETURN]) {
				onChar((char)2, false);
			}
			if (keyMapAdjusted[VK_CONTROL]) {
				if (i == 'V') {
					std::string str = util::GetClipboardText();
					for (char ch : str) {
						onChar(ch);
					}
				}
				else if (i == 'C') {
					onChar((char)1, false);
				}
				else if (i == 'A') {
					onChar(char(3), false);
				}
			}
			else {

				WORD wChars = 0;
				DWORD dwScanCode = 0;
				(reinterpret_cast<WORD*>(&dwScanCode))[1] = keyMapAdjusted[i];

				int res = ToAscii(i, dwScanCode, winKeyMap, &wChars, 0);
				if (res > 0) {
					char* chars = reinterpret_cast<char*>(&wChars);
					onChar(chars[0]);

					if (res == 2) { // If there's an extra character
						onChar(chars[1]);
					}
				}
			}
		}
	}

	if (ms > 350) {
		onChar('\b');
	}

	memcpy(keyMapOld, keyMapAdjusted, 0x100 * sizeof(int));
}

bool Keyboard::isKeyDown(int vKey) {
	return keyMap[vKey];
}

int Keyboard::getMappedKey(std::string const& name) {
	return SDK::ClientInstance::get()->inputHandler->mappingFactory->defaultKeyboardLayout->findValue(name);
}

void Keyboard::onChar(char ch, bool isChar) {
	CharEvent ev{ ch, isChar };
	Eventing::get().dispatch(ev);
}

void Keyboard::onKey(Event& evGeneric) {
	auto& ev = reinterpret_cast<KeyUpdateEvent&>(evGeneric);
	this->keyMapAdjusted[ev.getKey()] = ev.isDown();
}
