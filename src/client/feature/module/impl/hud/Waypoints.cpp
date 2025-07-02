#include "pch.h"
#include "Waypoints.h"

#include "sdk/common/client/gui/controls/UIControl.h"
#include "util/WorldToScreen.h"
#include "client/event/impl/KeyUpdateEvent.h"
#include "client/render/Renderer.h"
#include "client/Latite.h"
#include "client/screen/ScreenManager.h"

#include "client/misc/WaypointManager.h"

Waypoints::Waypoints() : Module("Waypoints", L"Waypoints", L"Show saved locations", HUD, nokeybind) {
    this->listen<RenderOverlayEvent>(&Waypoints::onRenderOverlay);
    this->listen<KeyUpdateEvent>(&Waypoints::onKey);

    addSetting("addWaypointKey", L"Add waypoint", L"", addWaypointKeySetting);
    addSetting("bgColor", L"Waypoint background color", L"", bgColorSetting);
}

void Waypoints::onKey(Event& evG) {
    KeyUpdateEvent& ev = reinterpret_cast<KeyUpdateEvent&>(evG);

    if (ev.isDown() && ev.getKey() == std::get<KeyValue>(addWaypointKeySetting) &&
        SDK::ClientInstance::get()->minecraftGame->isCursorGrabbed()) {
        ev.setCancelled(true);
        Latite::getScreenManager().showScreen<WaypointPopupScreen>();
    }
}

void Waypoints::onRenderOverlay(Event& evG) {
    if (!SDK::ClientInstance::get()->minecraftGame->isCursorGrabbed()) return;
    if (!SDK::ClientInstance::get()->getLocalPlayer()) return;
    if (!SDK::ClientInstance::get()->minecraft->getLevel()) return;
    if (!SDK::ClientInstance::get()->getLocalPlayer()->dimension) return;

    D2DUtil dc;
    SDK::LocalPlayer* localPlayer = SDK::ClientInstance::get()->getLocalPlayer();
    std::wstring currentDimension = util::StrToWStr(localPlayer->dimension->dimensionName);

    float deltaTime = Latite::get().getRenderer().getDeltaTime();
    float smoothingSpeed = 12.0f;
    float t = 1.0f - std::exp(-smoothingSpeed * deltaTime);
    
    for (const WaypointData& waypoint : Latite::get().getWaypointManager().getWaypoints()) {
        if (waypoint.dimension != currentDimension) continue;

        std::optional<Vec2> screenPos = WorldToScreen::convert(waypoint.position);
        if (!screenPos) continue;

        Vec2 smoothedPos = Latite::get().getWaypointManager().getSmoothedPosition(waypoint.position, *screenPos, t);

        float dist = waypoint.position.distance(localPlayer->getPos());

        std::wstringstream ss;
        ss.precision(1);
        ss << waypoint.initials << L"\n[" << static_cast<int>(dist) << L"m]";
        std::wstring text = ss.str();

        Vec2 textSize = dc.getTextSize(text, Renderer::FontSelection::PrimaryRegular, 14.f);
        float rectWidth = textSize.x + 20.f;
        float rectHeight = textSize.y + 10.f;

        d2d::Rect bgRect = {
            smoothedPos.x - rectWidth / 2.f,
            smoothedPos.y - rectHeight / 2.f,
            smoothedPos.x + rectWidth / 2.f,
            smoothedPos.y + rectHeight / 2.f
        };

        StoredColor bgColor = std::get<ColorValue>(bgColorSetting).getMainColor();

        dc.fillRoundedRectangle(bgRect, bgColor, 5.f);
        dc.drawText(bgRect, text, waypoint.color, Renderer::FontSelection::PrimaryRegular, 14.f,
            DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    }
}
