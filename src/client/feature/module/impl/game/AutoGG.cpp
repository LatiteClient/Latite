#include "pch.h"
#include "AutoGG.h"
#include <sdk/common/network/packet/CommandRequestPacket.h>
#include "sdk/common/network/MinecraftPackets.h"

AutoGG::AutoGG() : Module("AutoGG", LocalizeString::get("client.module.autoGG.name"),
                          LocalizeString::get("client.module.autoGG.desc"), GAME, nokeybind) {
    addSetting("useCustomMessage", LocalizeString::get("client.module.autoGG.useCustomMessage.name"),
               LocalizeString::get("client.module.autoGG.useCustomMessage.desc"), useCustomMessage);
    addSetting("customMessage", LocalizeString::get("client.module.autoGG.customMessage.name"),
               LocalizeString::get("client.module.autoGG.customMessage.desc"), customMessage, "useCustomMessage"_istrue);
    listen<ChatMessageEvent>(static_cast<EventListenerFunc>(&AutoGG::onText));
}

void AutoGG::onText(Event& evG) {
    ChatMessageEvent& ev = reinterpret_cast<ChatMessageEvent&>(evG);

    std::wstring chatMessage = util::StrToWStr(ev.getMessage());
    std::wstring hiveDetectMessage = L"\u00a7r\u00a7c\u00a7lGame OVER!";
    SDK::LocalPlayer* localPlayer = SDK::ClientInstance::get()->getLocalPlayer();

    if (localPlayer && chatMessage.find(hiveDetectMessage) != std::string::npos) {
        std::shared_ptr<SDK::Packet> pkt = SDK::MinecraftPackets::createPacket(SDK::PacketID::TEXT);
        SDK::TextPacket* tp = reinterpret_cast<SDK::TextPacket*>(pkt.get());

        if (std::get<BoolValue>(useCustomMessage)) {
            tp->chat(util::WStrToStr(std::get<TextValue>(customMessage).str));
        }
        else {
            tp->chat("gg");
        }
        localPlayer->packetSender->sendToServer(pkt.get());
    }
}
