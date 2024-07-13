#include "pch.h"
#include <cmath>
#include "CustomCoordinates.h"

CustomCoordinates::CustomCoordinates() : TextModule("CustomCoordinates",
                                                    LocalizeString::get("client.textmodule.customCoordinates.name"),
                                                    LocalizeString::get("client.textmodule.customCoordinates.desc"),
                                                    HUD) {
    addSetting("showDimension", LocalizeString::get("client.textmodule.customCoordinates.showDimension.name"),
               LocalizeString::get("client.textmodule.customCoordinates.showDimension.desc"), this->showDimension);
}

std::wstringstream CustomCoordinates::text(bool isDefault, bool inEditor) {
    if (!SDK::ClientInstance::get()->getLocalPlayer()) return std::wstringstream(L"");

    SDK::LocalPlayer* localPlayer = SDK::ClientInstance::get()->getLocalPlayer();

    int playerPosX = static_cast<int>(localPlayer->getPos().x);
    int playerPosY = lroundf(localPlayer->getPos().y - 1.62f); // very hacky fix to get vanilla y coordinate (probably not 100% accurate)
    int playerPosZ = static_cast<int>(localPlayer->getPos().z);
    std::wstring dimensionName = util::StrToWStr(localPlayer->dimension->dimensionName);

    std::wstring moduleText = util::StrToWStr(
        std::format(
            "X: {}\nY: {}\nZ: {}",
            playerPosX,
            playerPosY,
            playerPosZ
        ));

    if (std::get<BoolValue>(showDimension)) {
        if (dimensionName == L"Overworld")
            dimensionName = LocalizeString::get("client.textmodule.customCoordinates.dimensionDisplay.overworld.name");
        else if (dimensionName == L"Nether")
            dimensionName = LocalizeString::get("client.textmodule.customCoordinates.dimensionDisplay.nether.name");
        else if (dimensionName == L"TheEnd")
            dimensionName = LocalizeString::get("client.textmodule.customCoordinates.dimensionDisplay.theEnd.name");

        moduleText = util::FormatWString(L"X: {}\nY: {}\nZ: {}\n{}: {}", {
            std::to_wstring(playerPosX),
            std::to_wstring(playerPosY),
            std::to_wstring(playerPosZ),
            LocalizeString::get("client.textmodule.customCoordinates.dimension.name"),
            dimensionName
        });
    }

    return std::wstringstream(moduleText);
}