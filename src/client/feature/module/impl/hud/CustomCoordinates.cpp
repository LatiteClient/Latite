#include "pch.h"
#include <cmath>
#include "CustomCoordinates.h"

CustomCoordinates::CustomCoordinates() : TextModule("CustomCoordinates", "Custom Coordinates", "Shows player position and other info", HUD) {
    addSetting("showDimension", "Show Dimension", "Show the dimension the player is currently in", this->showDimension);
}

std::wstringstream CustomCoordinates::text(bool isDefault, bool inEditor) {
    if (!SDK::ClientInstance::get()->getLocalPlayer()) return std::wstringstream(L"");

    SDK::LocalPlayer* localPlayer = SDK::ClientInstance::get()->getLocalPlayer();

    int playerPosX = static_cast<int>(localPlayer->getPos().x);
    int playerPosY = lroundf(localPlayer->getPos().y - 1.62f); // very hacky fix to get vanilla y coordinate (probably not 100% accurate)
    int playerPosZ = static_cast<int>(localPlayer->getPos().z);
    std::string dimensionName = localPlayer->dimension->dimensionName;

    std::wstring moduleText = util::StrToWStr(
        std::format(
            "X: {}\nY: {}\nZ: {}",
            playerPosX,
            playerPosY,
            playerPosZ
        ));

    if (std::get<BoolValue>(showDimension)) {
        moduleText = util::StrToWStr(
            std::format(
                "X: {}\nY: {}\nZ: {}\nDimension: {}",
                playerPosX,
                playerPosY,
                playerPosZ,
                dimensionName
            ));
    }

    return std::wstringstream(moduleText);
}