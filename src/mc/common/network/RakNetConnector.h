#pragma once
#include "RakPeer.h"
#include "RemoteConnector.h"

namespace SDK {
    class RakNetConnector : public RemoteConnector {
    public:
        // please check for null
        static RakNetConnector* get();

        RakNetConnector() = delete;

        class RakPeer* getPeer() { return memory::callVirtual<RakPeer*>(this, 0x19); }
    };
}
