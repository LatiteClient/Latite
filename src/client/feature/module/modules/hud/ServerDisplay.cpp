#include "pch.h"
#include "ServerDisplay.h"
#include "mc/common/network/RemoteConnectorComposite.h"
ServerDisplay::ServerDisplay()
    : TextModule("ServerDisplay", LocalizeString::get("client.textmodule.serverDisplay.name"),
                 LocalizeString::get("client.textmodule.serverDisplay.desc"), HUD, 400.f, 0, true) {
    port.addEntry({ port_mixed, LocalizeString::get("client.textmodule.serverDisplay.showPortState0.name") });
    port.addEntry({ port_constant, LocalizeString::get("client.textmodule.serverDisplay.showPortState1.name") });
    port.addEntry({ port_none, LocalizeString::get("client.textmodule.serverDisplay.showPortState2.name") });
    addEnumSetting("showPort", LocalizeString::get("client.textmodule.serverDisplay.showPort.name"),
                   LocalizeString::get("client.textmodule.serverDisplay.showPort.desc"), port);
    addSetting("featuredName", LocalizeString::get("client.textmodule.serverDisplay.featuredName.name"),
               LocalizeString::get("client.textmodule.serverDisplay.featuredName.desc"), this->showServerName);
}

std::wstringstream ServerDisplay::text(bool isDefault, bool inEditor) {
    std::wstringstream wss;
    auto* connectionInfo = SDK::RemoteConnectorComposite::getConnectionInfo();
    if (connectionInfo && !connectionInfo->hostIpAddress.empty()) {
        if (!connectionInfo->thirdPartyServerInfo.creatorName.empty() && std::get<BoolValue>(showServerName)) {
            wss << util::StrToWStr(connectionInfo->thirdPartyServerInfo.creatorName);
        } else {
            const auto& address =
                connectionInfo->unresolvedUrl.empty() ? connectionInfo->hostIpAddress : connectionInfo->unresolvedUrl;
            wss << util::StrToWStr(address);
            if (port.getSelectedKey() == port_constant ||
                (port.getSelectedKey() == port_mixed && connectionInfo->port != 19132)) {
                wss << L":" << connectionInfo->port;
            }
        }
    } else
        wss << L"none";
    return wss;
}
