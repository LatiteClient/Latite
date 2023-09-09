#pragma once
#include "../Hooks.h"
#include "sdk/common/network/packet/SetTitlePacket.h"
#include "sdk/common/network/packet/TextPacket.h"

class PacketHooks : public HookGroup {
	static void* SetTitlePacket_readExtended(SDK::SetTitlePacket* pkt, void* b, void* c);
	static void* TextPacket_read(SDK::TextPacket* pkt, void* b, void*);
public:
	PacketHooks();
};