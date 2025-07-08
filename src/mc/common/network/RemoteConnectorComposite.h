#pragma once
#include "mc/Util.h"
#include "RakNetConnector.h"

namespace SDK {
    class RemoteConnectorComposite {
    public:
        CLASS_FIELD(RakNetConnector*, rakNetConnector, 0x70);
    };
}
