#pragma once

class PacketReceiveEvent : public Event {
public:
	static const uint32_t hash = TOHASH(PacketReceiveEvent);

	PacketReceiveEvent(SDK::Packet* pkt) : packet(pkt) {}

	[[nodiscard]] SDK::Packet* getPacket() { return packet; }

private:
	SDK::Packet* packet;
};
