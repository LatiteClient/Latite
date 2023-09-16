#include "pch.h"
#include "Keyboard.h"
#include "client/event/impl/CharEvent.h"
#include "sdk/common/client/input/ClientInputHandler.h"
#include <chrono>

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
					std::wstring str = util::GetClipboardText();
					for (wchar_t ch : str) {
						onChar(ch);
					}
				}
				else if (i == 'C') {
					onChar((wchar_t)1, false);
				}
				else if (i == 'A') {
					onChar(wchar_t(3), false);
				}
			}
			else {

				DWORD dwChars = 0;
				DWORD dwScanCode = 0;
				(reinterpret_cast<WORD*>(&dwScanCode))[1] = keyMapAdjusted[i];
				int res = ToUnicode(i, dwScanCode, winKeyMap, (LPWSTR) & dwChars, 4, 0);
				if (res > 0) {
					wchar_t* chars = reinterpret_cast<wchar_t*>(&dwChars);
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

void Keyboard::onChar(wchar_t ch, bool isChar) {
	CharEvent ev{ ch, isChar };
	Eventing::get().dispatch(ev);
}

void Keyboard::onKey(Event& evGeneric) {
	auto& ev = reinterpret_cast<KeyUpdateEvent&>(evGeneric);
	this->keyMapAdjusted[ev.getKey()] = ev.isDown();
}
