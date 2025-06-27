#include "pch.h"
#include "Waypoints.h"
#include "client/event/impl/RenderLayerEvent.h"
#include "sdk/common/client/gui/controls/VisualTree.h"
#include "sdk/common/client/gui/controls/UIControl.h"
#include "util/WorldToScreen.h"

Waypoints::Waypoints() : Module("Waypoints", L"Waypoints", L"Show saved locations", HUD, nokeybind) {
	this->listen<RenderOverlayEvent>(&Waypoints::onRenderOverlay);
}

void Waypoints::onRenderOverlay(Event& evG) {
	RenderOverlayEvent& ev = reinterpret_cast<RenderOverlayEvent&>(evG);
	//SDK::ScreenView* screenView = ev.getScreenView();

	if (!SDK::ClientInstance::get()->getLocalPlayer()) return;
	if (!SDK::ClientInstance::get()->minecraft->getLevel()) return;
	if (!SDK::ClientInstance::get()->getLocalPlayer()->dimension) return;
	//if (screenView->visualTree->rootControl->name != "hud_screen") return;
	if (!SDK::ClientInstance::get()->minecraftGame->isCursorGrabbed()) return;

	SDK::LocalPlayer* localPlayer = SDK::ClientInstance::get()->getLocalPlayer();

	int playerPosX = static_cast<int>(localPlayer->getPos().x);
	int playerPosY = lroundf(localPlayer->getPos().y - 1.62f); // very hacky fix to get vanilla y coordinate (probably not 100% accurate)
	int playerPosZ = static_cast<int>(localPlayer->getPos().z);
	std::wstring levelName = util::StrToWStr(SDK::ClientInstance::get()->minecraft->getLevel()->name);
	std::wstring dimensionName = util::StrToWStr(localPlayer->dimension->dimensionName);
	Vec3 origin = SDK::ClientInstance::get()->levelRenderer->getLevelRendererPlayer()->getOrigin();
	Latite::get().getRenderer().updateSecondaryFont(L"Mojangles");

	D2DUtil dc;
	Vec3 waypoint = Vec3(0, -60, 0); // dummy waypoint for testing
	static std::optional<Vec2> smoothedScreenPos = std::nullopt;
	static constexpr float smoothingFactor = 0.9f;

	std::optional<Vec2> screenPos = WorldToScreen::convert(waypoint);

	if (screenPos != std::nullopt) {
		float playerDistance = waypoint.distance(origin);

		if (smoothedScreenPos == std::nullopt) {
			smoothedScreenPos = screenPos;
		}
		else {
			smoothedScreenPos->x += (screenPos->x - smoothedScreenPos->x) * smoothingFactor * Latite::get().getRenderer().getDeltaTime();
			smoothedScreenPos->y += (screenPos->y - smoothedScreenPos->y) * smoothingFactor * Latite::get().getRenderer().getDeltaTime();
		}

		float rectWidth = 40.0f;
		float rectHeight = 40.0f;

		float rectRight = smoothedScreenPos->x + rectWidth;
		float rectBottom = smoothedScreenPos->y + rectHeight;

		dc.fillRoundedRectangle({ smoothedScreenPos->x, smoothedScreenPos->y, rectRight, rectBottom }, d2d::Colors::BLACK);
        dc.drawText({ smoothedScreenPos->x, smoothedScreenPos->y, rectRight, rectBottom }, L"M", d2d::Colors::WHITE,
                    Renderer::FontSelection::SecondaryRegular, 25, DWRITE_TEXT_ALIGNMENT_CENTER,
                    DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	}

}
