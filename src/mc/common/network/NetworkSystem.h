#pragma once

#include "NetworkConnection.h"
#include "mc/Util.h"
#include "RemoteConnectorComposite.h"

namespace SDK {
    class NetworkSystem {
    public:
        CLASS_FIELD(RemoteConnectorComposite*, remoteConnector, 0x120);

        std::vector<std::unique_ptr<NetworkConnection>>& getConnections() {
            return hat::member_at<std::vector<std::unique_ptr<NetworkConnection>>>(this, 0xE8);
        }
    };
}
