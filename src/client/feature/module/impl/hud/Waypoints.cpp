#include "pch.h"
#include "Waypoints.h"

#include "sdk/common/client/gui/controls/UIControl.h"
#include "util/WorldToScreen.h"

Waypoints::Waypoints() : Module("Waypoints", L"Waypoints", L"Show saved locations", HUD, nokeybind) {
    this->listen<RenderOverlayEvent>(&Waypoints::onRenderOverlay);

    addSetting("bgColor", L"Waypoint background color", L"", bgColorSetting);
    addSetting("textColor", L"Waypoint text color", L"", textColorSetting);
}

void Waypoints::onRenderOverlay(Event& evG) {
    if (!SDK::ClientInstance::get()->getLocalPlayer()) return;
    if (!SDK::ClientInstance::get()->minecraft->getLevel()) return;
    if (!SDK::ClientInstance::get()->getLocalPlayer()->dimension) return;
    if (!SDK::ClientInstance::get()->minecraftGame->isCursorGrabbed()) return;

    D2DUtil dc;

    SDK::LocalPlayer* localPlayer = SDK::ClientInstance::get()->getLocalPlayer();
    std::wstring levelName = util::StrToWStr(SDK::ClientInstance::get()->minecraft->getLevel()->name);
    std::wstring dimensionName = util::StrToWStr(localPlayer->dimension->dimensionName);

    Vec3 waypoint = Vec3(0, -60, 0); // dummy waypoint for testing

    std::optional<Vec2> screenPos = WorldToScreen::convert(waypoint);

    if (screenPos != std::nullopt) {
        if (smoothedScreenPos == std::nullopt) {
            smoothedScreenPos = screenPos;
        }

        if (smoothedScreenPos && screenPos) {
            constexpr float smoothingFactor = 0.9f;
            float deltaTime = Latite::get().getRenderer().getDeltaTime();
            smoothedScreenPos->x += (screenPos->x - smoothedScreenPos->x) * smoothingFactor * deltaTime;
            smoothedScreenPos->y += (screenPos->y - smoothedScreenPos->y) * smoothingFactor * deltaTime;
        }

        float rectWidth = 40.0f;
        float rectHeight = 40.0f;

        float rectRight = smoothedScreenPos->x + rectWidth;
        float rectBottom = smoothedScreenPos->y + rectHeight;

        StoredColor textColor = std::get<ColorValue>(textColorSetting).getMainColor();
        StoredColor bgColor = std::get<ColorValue>(bgColorSetting).getMainColor();

        dc.fillRoundedRectangle({ smoothedScreenPos->x, smoothedScreenPos->y, rectRight, rectBottom }, bgColor);
        dc.drawText({ smoothedScreenPos->x, smoothedScreenPos->y, rectRight, rectBottom }, L"M", textColor,
                    Renderer::FontSelection::PrimaryRegular, 25, DWRITE_TEXT_ALIGNMENT_CENTER,
                    DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    }
}
