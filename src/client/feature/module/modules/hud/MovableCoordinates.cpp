#include "pch.h"
#include "MovableCoordinates.h"
#include <mc/common/client/gui/ScreenView.h>
#include <mc/common/client/gui/controls/VisualTree.h>
#include <mc/common/client/gui/controls/UIControl.h>

MovableCoordinates::MovableCoordinates() : HUDModule("MovableCoordinates",
                                                     LocalizeString::get("client.hudmodule.movableCoordinates.name"),
                                                     LocalizeString::get("client.hudmodule.movableCoordinates.desc"),
                                                     HUD, 0, false) {
    listen<RenderLayerEvent>(static_cast<EventListenerFunc>(&MovableCoordinates::onRenderLayer), true,
                             10 /*need to overpower the hud renderer*/);
    addSetting("hideVanillaCoordinates",
               LocalizeString::get("client.hudmodule.movableCoordinates.hideVanillaCoordinates.name"), L"",
               this->hideVanillaCoordinates);
}

void MovableCoordinates::render(DrawUtil& ctx, bool isDefault, bool inEditor) {
}

void MovableCoordinates::onRenderLayer(Event& evG) {
    RenderLayerEvent& ev = reinterpret_cast<RenderLayerEvent&>(evG);
    SDK::LocalPlayer* localPlayer = SDK::ClientInstance::get()->getLocalPlayer();

    if (!localPlayer) {
        this->vanillaCoordinates = nullptr;
        return;
    }

    if (this->isActive() && this->isEnabled()) {
        if (ev.getScreenView()->visualTree->rootControl->name == XOR_STRING("hud_screen")) {
            this->vanillaCoordinates = ev.getScreenView()->visualTree->rootControl->findFirstDescendantWithName(
                XOR_STRING("player_position"));
            float guiScale = SDK::ClientInstance::get()->getGuiData()->guiScale;


            if (this->vanillaCoordinates) {
                if (std::get<BoolValue>(this->hideVanillaCoordinates)) {
                    vanillaCoordinates->position = { 9999.f, 9999.f }; // very scuffed
                    updatePos();
                }
                else {
                    vanillaCoordinates->position = { rect.left / guiScale, rect.top / guiScale };
                    updatePos();

                    this->rect.right = rect.left + vanillaCoordinates->bounds.x * guiScale;
                    this->rect.bottom = rect.top + vanillaCoordinates->bounds.y * guiScale;
                }
            }
        }
    }
    else if (!this->isEnabled() && this->vanillaCoordinates) {
        vanillaCoordinates->position = { 3.f, 52.f }; // default vanilla coordinates position
        updatePos();
    }
}

void MovableCoordinates::updatePos() {
    vanillaCoordinates->getDescendants([](std::shared_ptr<SDK::UIControl> control) {
        control->updatePos();
    });
}
