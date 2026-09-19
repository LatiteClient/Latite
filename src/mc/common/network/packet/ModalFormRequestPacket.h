#pragma once
#include "../Packet.h"

namespace SDK {

    class ModalFormRequestPacket : public Packet {
    public:
        uint32_t mFormId;
        std::string mFormJSON;
    };
}
