#pragma once

#include "NetworkPeer.h"
#include "../../Util.h"

namespace SDK {
    class NetworkConnection {
    public:
        CLASS_FIELD(NetworkPeer*, networkPeer, 0x118);
        CLASS_FIELD(bool, shouldSkip, 0x199);
    };
}
