#pragma once
#include <mc/Addresses.h>

#pragma optimize("", off)
namespace SDK {
	class MinecraftPackets {
	public:
		static std::shared_ptr<Packet> createPacket(PacketID id) {
			return reinterpret_cast<std::shared_ptr<Packet>(*)(PacketID)>(Signatures::MinecraftPackets_createPacket.result)(id);
		}
	};
}
#pragma optimize("", on)