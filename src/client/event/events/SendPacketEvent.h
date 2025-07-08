#pragma once
#include "client/event/Event.h"
#include "util/Crypto.h"
#include <mc/common/network/Packet.h>

class SendPacketEvent : public Cancellable {
public:
	static const uint32_t hash = TOHASH(SendPacketEvent);

	[[nodiscard]] SDK::Packet* getPacket() {
		return packet;
	}

	SendPacketEvent(SDK::Packet* pkt) : packet(pkt) {}
protected:
	SDK::Packet* packet;
};
