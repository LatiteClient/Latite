#include "pch.h"
#include "PingDisplay.h"
#include "mc/common/network/RemoteConnectorComposite.h"
#include "client/event/events/AveragePingEvent.h"
#include "client/event/Eventing.h"

PingDisplay::PingDisplay()
    : TextModule("PingDisplay", LocalizeString::get("client.textmodule.pingDisplay.name"),
                 LocalizeString::get("client.textmodule.pingDisplay.desc"), HUD) {
    this->suffix = TextValue(L" ms");

    listen<AveragePingEvent>((EventListenerFunc)&PingDisplay::onAvgPing, true);
}

std::wstringstream PingDisplay::text(bool isDefault, bool inEditor) {
    std::wstringstream wss;
    int dPing = 0;

    auto* connectionInfo = SDK::RemoteConnectorComposite::getConnectionInfo();
    if (connectionInfo && !connectionInfo->hostIpAddress.empty()) {
        bool found = false;

        for (const auto& connection : SDK::ClientInstance::get()->packetSender->networkSystem->getConnections()) {
            if (connection == nullptr || connection->shouldSkip || connection->networkPeer == nullptr)
                continue;

            dPing = static_cast<int>(connection->networkPeer->getNetworkStatus().currentPing);
            found = true;
            break;
        }

        if (!found)
            dPing = ping;
    }

    wss << dPing;

    return wss;
}

void PingDisplay::onAvgPing(Event& evGeneric) {
    AveragePingEvent& ev = static_cast<AveragePingEvent&>(evGeneric);

    ping = ev.getPing();
}
