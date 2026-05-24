#pragma once
#include "../Packet.h"

namespace SDK {
	class SetActorDataPacket : public Packet {
	public:
		bool tryGetNameTag(uint64_t* runtimeId, std::string* nameTag) const;
	};
}
