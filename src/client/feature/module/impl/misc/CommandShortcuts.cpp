#include "pch.h"
#include "CommandShortcuts.h"
#include <sdk/common/network/packet/CommandRequestPacket.h>

CommandShortcuts::CommandShortcuts() : Module("CommandShortcuts",
                                              LocalizeString::get("client.module.commandShortcuts.name"),
                                              LocalizeString::get("client.module.commandShortcuts.desc"), GAME) {
    listen<SendPacketEvent>((EventListenerFunc)&CommandShortcuts::onPacket);
}

void CommandShortcuts::onPacket(Event& evG) {
    auto& ev = reinterpret_cast<SendPacketEvent&>(evG);
    auto packetId = ev.getPacket()->getID();

    if (packetId == SDK::PacketID::COMMAND_REQUEST) {
        SDK::CommandRequestPacket* pkt = reinterpret_cast<SDK::CommandRequestPacket*>(ev.getPacket());

        if (pkt->command == "/gmsp") {
            pkt->command = "/gamemode spectator";
        }
        else if (pkt->command == "/gmc") {
            pkt->command = "/gamemode creative";
        }
        else if (pkt->command == "/gms") {
            pkt->command = "/gamemode survival";
        }
        else if (pkt->command == "/gma") {
            pkt->command = "/gamemode adventure";
        }
    }
}
