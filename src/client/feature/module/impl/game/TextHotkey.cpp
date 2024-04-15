#include "pch.h"
#include "TextHotkey.h"

TextHotkey::TextHotkey() : Module("TextHotkey", "Text Hotkey", "Binds a chat message to a text message.", GAME, nokeybind) {
	addSetting("textKey", "Hotkey", "The hotkey to bind the text message to", this->textKey);
	listen<KeyUpdateEvent>((EventListenerFunc)&onKey);
}

void TextHotkey::onKey(Event& evG) {
	auto& ev = reinterpret_cast<KeyUpdateEvent&>(evG);

	if (ev.isDown() && ev.getKey() == std::get<KeyValue>(this->textKey)) {
		auto now = std::chrono::system_clock::now();

		if (now - lastSend > 2s) {
			SDK::TextPacket pkt{};

			String str{};
			str.setString(util::WStrToStr(std::get<TextValue>(this->textMessage).str).c_str());
			pkt.chat(str);
			lastSend = now;
		}
	}
}
