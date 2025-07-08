#pragma once
#include <client/feature/module/Module.h>

class TextHotkey : public Module {
public:
	TextHotkey();
	virtual ~TextHotkey() {};

	void onKey(Event&);

private:
	ValueType textKey = KeyValue('H');
	ValueType textMessage = TextValue(L"Latite Client On Top");
	ValueType commandMode = BoolValue(false);

	std::chrono::system_clock::time_point lastSend;
};