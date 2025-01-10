#pragma once
#include "../Packet.h"
#include "sdk/String.h"

namespace SDK {
    class ModalFormRequestPacket : public Packet {
    public:
        unsigned int   Id;
        std::string    Json;
    };
}