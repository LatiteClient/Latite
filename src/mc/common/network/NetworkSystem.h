#pragma once
#include "mc/Util.h"
#include "RemoteConnectorComposite.h"

namespace SDK {
    class NetworkSystem {
    public:
        CLASS_FIELD(RemoteConnectorComposite*, remoteConnector, 0xF0);
    };
}
