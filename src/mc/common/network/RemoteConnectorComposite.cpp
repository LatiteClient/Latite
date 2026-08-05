#include "RemoteConnectorComposite.h"
#include "PacketSender.h"
#include "mc/common/client/game/ClientInstance.h"

SDK::RemoteConnectorComposite* SDK::RemoteConnectorComposite::get() {
    auto* clientInstance = ClientInstance::get();
    if (!clientInstance || !clientInstance->packetSender || !clientInstance->packetSender->networkSystem) {
        return nullptr;
    }

    return clientInstance->packetSender->networkSystem->remoteConnector;
}

SDK::RemoteConnector* SDK::RemoteConnectorComposite::getActiveConnector() {
    auto* ownerControlBlock = hat::member_at<uint8_t*>(this, 0x50);
    auto* networkSessionOwner = hat::member_at<void*>(this, 0x60);
    if (!ownerControlBlock || !*ownerControlBlock || !networkSessionOwner) {
        return nullptr;
    }

    auto* sessionInfo = hat::member_at<void*>(networkSessionOwner, 0x18);
    const bool usesNetherNet = sessionInfo && hat::member_at<int>(sessionInfo, 0x18) == 2;
    return usesNetherNet ? static_cast<RemoteConnector*>(netherNetConnector)
                         : static_cast<RemoteConnector*>(rakNetConnector);
}

SDK::Social::GameConnectionInfo* SDK::RemoteConnectorComposite::getConnectionInfo() {
    auto* composite = get();
    if (!composite) return nullptr;

    auto* connector = composite->getActiveConnector();
    return connector ? connector->getConnectedGameInfo() : nullptr;
}
