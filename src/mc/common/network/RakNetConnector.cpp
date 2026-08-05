#include "RakNetConnector.h"
#include "RemoteConnectorComposite.h"

SDK::RakNetConnector* SDK::RakNetConnector::get() {
    auto* composite = RemoteConnectorComposite::get();
    return composite ? composite->rakNetConnector : nullptr;
}
