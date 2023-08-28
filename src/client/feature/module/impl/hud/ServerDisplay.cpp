#include "ServerDisplay.h"
#include "sdk/common/network/RakNetConnector.h"
#include "pch.h"

ServerDisplay::ServerDisplay() : TextModule("ServerDisplay", "Server Display", "Displays what server you are on", HUD) {
    port.addEntry({ port_mixed, "Non-Default" });
    port.addEntry({ port_constant, "Always"});
    port.addEntry({ port_none, "None" });
    addEnumSetting("showPort", "Show Port", "How to show the port", port);
    addSetting("featuredName", "Featured Server Name", "Whether to show the featured server name or not", this->showServerName);
}

std::wstringstream ServerDisplay::text(bool isDefault, bool inEditor) {
    std::wstringstream wss;
    auto connector = SDK::RakNetConnector::get();
    if (connector && connector->ipAddress.size() > 0) {
        if (!connector->featuredServer.empty()) {
            wss << util::StrToWStr(connector->featuredServer);
        }
        else {
            wss << util::StrToWStr(connector->dns);
            if (port.getSelectedKey() == port_constant || (port.getSelectedKey() == port_mixed && connector->port != 19132)) {
                wss << L":" << connector->port;
            }
        }
    } else wss << L"none";
    return wss;
}
