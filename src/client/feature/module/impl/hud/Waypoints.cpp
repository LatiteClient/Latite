#include "pch.h"
#include "Waypoints.h"
#include "client/event/impl/RenderLayerEvent.h"
#include "sdk/common/client/gui/controls/VisualTree.h"
#include "sdk/common/client/gui/controls/UIControl.h"
#include "util/WorldToScreen.h"

Waypoints::Waypoints() : Module("Waypoints", L"Waypoints", L"Show saved locations", HUD, nokeybind) {
	this->listen<RenderLayerEvent>(&Waypoints::onRenderLayer);
}

void Waypoints::onRenderLayer(Event& evG) {
	RenderLayerEvent& ev = reinterpret_cast<RenderLayerEvent&>(evG);
	SDK::ScreenView* screenView = ev.getScreenView();

	if (!SDK::ClientInstance::get()->getLocalPlayer()) return;
	if (!SDK::ClientInstance::get()->minecraft->getLevel()) return;
	if (!SDK::ClientInstance::get()->getLocalPlayer()->dimension) return;
	if (screenView->visualTree->rootControl->name != "hud_screen") return;

	SDK::LocalPlayer* localPlayer = SDK::ClientInstance::get()->getLocalPlayer();

	int playerPosX = static_cast<int>(localPlayer->getPos().x);
	int playerPosY = lroundf(localPlayer->getPos().y - 1.62f); // very hacky fix to get vanilla y coordinate (probably not 100% accurate)
	int playerPosZ = static_cast<int>(localPlayer->getPos().z);
	std::wstring levelName = util::StrToWStr(SDK::ClientInstance::get()->minecraft->getLevel()->name);
	std::wstring dimensionName = util::StrToWStr(localPlayer->dimension->dimensionName);
	Vec3 origin = SDK::ClientInstance::get()->levelRenderer->getLevelRendererPlayer()->getOrigin();

	MCDrawUtil dc{ ev.getUIRenderContext(), SDK::ClientInstance::get()->minecraftGame->minecraftFont };
}
