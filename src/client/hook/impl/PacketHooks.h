#pragma once
#include "../Hook.h"
#include "sdk/common/network/packet/SetTitlePacket.h"
#include "sdk/common/network/packet/TextPacket.h"

class PacketHooks : public HookGroup {
	static void* SetTitlePacket_readExtended(SDK::SetTitlePacket* pkt, void* b, void* c);
	static void* TextPacket_read(SDK::TextPacket* pkt, void* b, void*);
	static void PacketSender_sendToServer(SDK::PacketSender* sender, SDK::Packet* packet);
	static std::shared_ptr<SDK::Packet> MinecraftPackets_createPacket(SDK::PacketID packetId);
public:
	PacketHooks();

	void initPacketSender(SDK::PacketSender* sender);
};