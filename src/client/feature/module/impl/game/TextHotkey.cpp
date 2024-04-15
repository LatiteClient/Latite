#include "pch.h"
#include "TextHotkey.h"
#include <sdk/common/network/packet/CommandRequestPacket.h>

TextHotkey::TextHotkey() : Module("TextHotkey", "Text/Command Hotkey", "Binds a chat message or a command to a text message.", GAME, nokeybind) {
	addSetting("commandMode", "Command Mode", "Send a command instead of a chat message", this->commandMode);
	addSetting("textKey", "Hotkey", "The hotkey to bind the text message or command to", this->textKey);
	addSetting("text", "Text", "The text or command", this->textKey);
	listen<KeyUpdateEvent>((EventListenerFunc)&onKey);
}

void TextHotkey::onKey(Event& evG) {
	auto& ev = reinterpret_cast<KeyUpdateEvent&>(evG);

	if (ev.isDown() && ev.getKey() == std::get<KeyValue>(this->textKey)) {
		auto now = std::chrono::system_clock::now();

		if (now - lastSend > 2s) {
			auto msg = util::WStrToStr(std::get<TextValue>(this->textMessage).str);
			if (msg.size() < 1 || msg.size() > 200) {
				return;
			}

			if (std::get<BoolValue>(commandMode)) {
				SDK::CommandRequestPacket pkt = SDK::CommandRequestPacket("/" + msg);
				SDK::ClientInstance::get()->getLocalPlayer()->packetSender->sendToServer(pkt);
				lastSend = now;
			}
			else {
				SDK::TextPacket pkt{};

				String str{};
				str.setString(util::WStrToStr(std::get<TextValue>(this->textMessage).str).c_str());
				pkt.chat(str);

				SDK::ClientInstance::get()->getLocalPlayer()->packetSender->sendToServer(pkt);

				lastSend = now;
			}
		}
	}
}
