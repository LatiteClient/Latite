#pragma once
#include "../Packet.h"

namespace SDK {
    class AddPlayerPacket : public Packet {
    public:
        bool tryGetNameTag(uint64_t* runtimeId, std::string* nameTag) const;
    };
}
