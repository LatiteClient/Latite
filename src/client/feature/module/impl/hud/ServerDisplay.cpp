#include "pch.h"
#include "ServerDisplay.h"
#include "sdk/common/network/RakNetConnector.h"
ServerDisplay::ServerDisplay() : TextModule("ServerDisplay",
                                            LocalizeString::get("client.textmodule.serverDisplay.name"),
                                            LocalizeString::get("client.textmodule.serverDisplay.desc"), HUD, 400.f, 0,
                                            true) {
    port.addEntry({port_mixed, LocalizeString::get("client.textmodule.serverDisplay.showPortState0.name")});
    port.addEntry({port_constant, LocalizeString::get("client.textmodule.serverDisplay.showPortState1.name")});
    port.addEntry({port_none, LocalizeString::get("client.textmodule.serverDisplay.showPortState2.name")});
    addEnumSetting("showPort", LocalizeString::get("client.textmodule.serverDisplay.showPort.name"),
                   LocalizeString::get("client.textmodule.serverDisplay.showPort.desc"), port);
    addSetting("featuredName", LocalizeString::get("client.textmodule.serverDisplay.featuredName.name"),
               LocalizeString::get("client.textmodule.serverDisplay.featuredName.desc"), this->showServerName);
}



std::wstringstream ServerDisplay::text(bool isDefault, bool inEditor) {
    std::wstringstream wss;
    auto connector = SDK::RakNetConnector::get();
    if (connector && connector->ipAddress.size() > 0) {
        if (!connector->featuredServer.empty() && std::get<BoolValue>(showServerName)) {
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
