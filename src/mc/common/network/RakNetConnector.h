#pragma once
#include "mc/Util.h"
#include "util/memory.h"
#include "RakPeer.h"

#include <string>

namespace SDK {
    class RakNetConnector {
    public:
        // please check for null
        static RakNetConnector* get();

        RakNetConnector() = delete;

        CLASS_FIELD(std::string, ipAddress, 0x400);
        CLASS_FIELD(std::string, dns, 0x420);
        CLASS_FIELD(uint16_t, port, 0x464);
        CLASS_FIELD(std::string, featuredServer, 0x508);

        class RakPeer* getPeer() { return memory::callVirtual<RakPeer*>(this, 0x19); }
    };
}
