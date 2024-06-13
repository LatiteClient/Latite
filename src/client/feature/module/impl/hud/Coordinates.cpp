#include "pch.h"
#include <cmath>
#include "Coordinates.h"

Coordinates::Coordinates() : TextModule("Coordinates", "Coordinates", "Shows player position and other info", HUD) {
    addSetting("showBiome", "Show Biome", "Show the biome the player is currently in", this->showBiome);
    addSetting("showDimension", "Show Dimension", "Show the dimension the player is currently in", this->showDimension);
}

std::wstringstream Coordinates::text(bool isDefault, bool inEditor) {
    SDK::LocalPlayer* localPlayer = SDK::ClientInstance::get()->getLocalPlayer();

    int playerPosX = localPlayer->getPos().x;
    int playerPosY = lroundf(localPlayer->getPos().y - 1.62f); // very hacky fix to get vanilla y coordinate (not 100% accurate)
    int playerPosZ = localPlayer->getPos().z;

    std::wstring moduleText = util::StrToWStr(
        std::format(
            "X: {}\nY: {}\nZ: {}",
            playerPosX,
            playerPosY,
            playerPosZ
            ));

    return std::wstringstream(moduleText);
}