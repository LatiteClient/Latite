#pragma once
#include "mc/Util.h"
#include "NetherNetConnector.h"
#include "RakNetConnector.h"

namespace SDK {
    class RemoteConnectorComposite {
    public:
        static RemoteConnectorComposite* get();
        static Social::GameConnectionInfo* getConnectionInfo();

        RemoteConnector* getActiveConnector();

        CLASS_FIELD(NetherNetConnector*, netherNetConnector, 0x68);
        CLASS_FIELD(RakNetConnector*, rakNetConnector, 0x70);
    };
}
