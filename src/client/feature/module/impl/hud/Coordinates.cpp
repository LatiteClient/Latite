#include "pch.h"
#include <cmath>
#include "Coordinates.h"
#include "sdk/common/client/gui/controls/VisualTree.h"

Coordinates::Coordinates() : TextModule("Coordinates", "Coordinates", "Shows player position and other info", HUD) {
    addSetting("showDimension", "Show Dimension", "Show the dimension the player is currently in", this->showDimension);
    addSetting("hideVanillaCoordinates", "Hide vanilla coordinates", "", this->hideVanillaCoordinates);
    addSetting(
        "movableVanillaCoordinates",
        "Movable vanilla coordinates",
        "Instead of using Latite's coordinates, have vanilla coordinates be movable",
        this->movableVanillaCoordinates);
    listen<RenderLayerEvent>(static_cast<EventListenerFunc>(&Coordinates::onRenderLayer), true,
                             10 /*need to overpower the hud renderer*/);
}

std::wstringstream Coordinates::text(bool isDefault, bool inEditor) {
    if (!SDK::ClientInstance::get()->getLocalPlayer()) return std::wstringstream(L"");
    if (std::get<BoolValue>(this->movableVanillaCoordinates)) return std::wstringstream(L"");

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

void Coordinates::onRenderLayer(Event& evG) {
    RenderLayerEvent& ev = reinterpret_cast<RenderLayerEvent&>(evG);
    SDK::LocalPlayer* localPlayer = SDK::ClientInstance::get()->getLocalPlayer();

    if (!localPlayer) {
        this->vanillaCoordinates = nullptr;
        return;
    }

    if (this->isActive() && this->isEnabled()) {
        if (ev.getScreenView()->visualTree->rootControl->name == XOR_STRING("hud_screen")) {
            this->vanillaCoordinates = ev.getScreenView()->visualTree->rootControl->findFirstDescendantWithName(
                XOR_STRING("player_position_text"));
            float guiScale = SDK::ClientInstance::get()->getGuiData()->guiScale;


            if (this->vanillaCoordinates) {
                if (std::get<BoolValue>(this->hideVanillaCoordinates)) {
                    vanillaCoordinates->position = { 9999.f, 9999.f }; // very scuffed
                    updatePos();
                }
                else if (std::get<BoolValue>(this->movableVanillaCoordinates)) {
                    vanillaCoordinates->position = { rect.left / guiScale, rect.top / guiScale };
                    updatePos();

                    this->rect.right = rect.left + vanillaCoordinates->bounds.x * guiScale;
                    this->rect.bottom = rect.top + vanillaCoordinates->bounds.y * guiScale;
                }
                else {
                    vanillaCoordinates->position = { 3.f, 52.f }; // default vanilla coordinate position
                    updatePos();
                }
            }
        }
    }
}

void Coordinates::updatePos() {
    vanillaCoordinates->getDescendants([](std::shared_ptr<SDK::UIControl> control) {
        control->updatePos();
    });
}
