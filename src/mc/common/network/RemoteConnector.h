#pragma once
#include "GameConnectionInfo.h"
#include "util/memory.h"

namespace SDK {
    class RemoteConnector {
    public:
        Social::GameConnectionInfo* getConnectedGameInfo() {
            return memory::callVirtual<Social::GameConnectionInfo*>(this, 0x3);
        }
    };
}
