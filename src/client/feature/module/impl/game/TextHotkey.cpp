#include "pch.h"
#include "TextHotkey.h"
#include <sdk/common/network/packet/CommandRequestPacket.h>

#include "sdk/common/network/MinecraftPackets.h"

TextHotkey::TextHotkey() : Module("TextHotkey", LocalizeString::get("client.module.textHotkey.name"),
                                  LocalizeString::get("client.module.textHotkey.desc"), GAME, nokeybind) {
    addSetting("commandMode", LocalizeString::get("client.module.textHotkey.commandMode.name"),
               LocalizeString::get("client.module.textHotkey.commandMode.desc"), this->commandMode);
    addSetting("textKey", LocalizeString::get("client.module.textHotkey.textKey.name"),
               LocalizeString::get("client.module.textHotkey.textKey.desc"), this->textKey);
    addSetting("text", LocalizeString::get("client.module.textHotkey.text.name"),
               LocalizeString::get("client.module.textHotkey.text.desc"), this->textMessage);
    listen<KeyUpdateEvent>(static_cast<EventListenerFunc>(&TextHotkey::onKey));
}

void TextHotkey::onKey(Event& evG) {
	auto& ev = reinterpret_cast<KeyUpdateEvent&>(evG);

	if (ev.isDown() && ev.getKey() == std::get<KeyValue>(this->textKey) && !ev.inUI()) {
		auto now = std::chrono::system_clock::now();

		if (now - lastSend > 2s) {
			auto msg = util::WStrToStr(std::get<TextValue>(this->textMessage).str);
			if (msg.size() < 1 || msg.size() > 200) {
				return;
			}

			if (std::get<BoolValue>(commandMode)) {
				auto pkt = SDK::MinecraftPackets::createPacket(SDK::PacketID::COMMAND_REQUEST);
				SDK::CommandRequestPacket* cmd = reinterpret_cast<SDK::CommandRequestPacket*>(pkt.get());
				cmd->applyCommand("/"+msg);
				SDK::ClientInstance::get()->getLocalPlayer()->packetSender->sendToServer(pkt.get());
				lastSend = now;
			}
			else {
				auto pkt = SDK::MinecraftPackets::createPacket(SDK::PacketID::TEXT);
				SDK::TextPacket* tp = reinterpret_cast<SDK::TextPacket*>(pkt.get());
				
				tp->chat(msg);

				SDK::ClientInstance::get()->getLocalPlayer()->packetSender->sendToServer(pkt.get());

				lastSend = now;
			}
		}
	}
}
