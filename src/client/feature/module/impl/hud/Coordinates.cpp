#include "pch.h"
#include <cmath>
#include "Coordinates.h"
#include <util/DxContext.h>

#include "sdk/common/client/gui/controls/VisualTree.h"

Coordinates::Coordinates() : HUDModule("Coordinates", "Coordinates", "Shows player position and other info", HUD) {
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

void Coordinates::render(DrawUtil& dc, bool isDefault, bool inEditor) {
	// todo: fix module looking weird in mod menu dropdown (rect issue)

	if (!SDK::ClientInstance::get()->getLocalPlayer()) return;
	if (std::get<BoolValue>(this->movableVanillaCoordinates)) return;

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
	std::wstring dimensionModuleText = util::StrToWStr(std::format("Dimension: {}", dimensionName));

	Vec2 moduleTextSize = dc.getTextSize(moduleText, Renderer::FontSelection::PrimaryRegular, 28.f);
	Vec2 dimensionTextSize = dc.getTextSize(dimensionModuleText, Renderer::FontSelection::PrimaryRegular, 28.f);

	d2d::Rect moduleRc = d2d::Rect(0.f, 0.f, moduleTextSize.x, moduleTextSize.y);
	rect.right = rect.left + moduleRc.right;
	rect.bottom = rect.top + moduleRc.bottom;

	dc.drawText(moduleRc, moduleText, d2d::Colors::WHITE, Renderer::FontSelection::PrimaryRegular,
	            28.f, DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_NEAR, false);

	if (std::get<BoolValue>(showDimension)) {
		Color dimensionTextColor;

		if (dimensionName == "Overworld") dimensionTextColor = d2d::Colors::WHITE;
		else if (dimensionName == "Nether") dimensionTextColor = d2d::Color::RGB(206, 70, 70);
		else if (dimensionName == "TheEnd") {
			dimensionTextColor = d2d::Color::RGB(249, 242, 196);
			dimensionName = "The End";
		}

		d2d::Rect dimensionRc = d2d::Rect(0.f, 120.f, dimensionTextSize.x, dimensionTextSize.y + 15.f);
		// for some reason the rect goes out of bounds of the hud preview on mc renderer, so we add more to y here
		if (std::get<BoolValue>(this->forceMCRend))
			dimensionRc = d2d::Rect(0.f, 120.f, dimensionTextSize.x, dimensionTextSize.y + 35.f);
		rect.right = rect.left + moduleRc.right + dimensionRc.right;
		rect.bottom = rect.top + moduleRc.bottom + dimensionRc.bottom;

		dc.drawText(dimensionRc, dimensionModuleText, dimensionTextColor, Renderer::FontSelection::PrimaryRegular,
		            28.f, DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_NEAR, false);
	}

	dc.flush(true, false);
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

			if (this->vanillaCoordinates && std::get<BoolValue>(this->hideVanillaCoordinates)) {
				vanillaCoordinates->position = { 9999.f, 9999.f }; // very scuffed
				updatePos();
			}
			else if (this->vanillaCoordinates && std::get<BoolValue>(this->movableVanillaCoordinates)) {
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

void Coordinates::updatePos() {
	vanillaCoordinates->getDescendants([](std::shared_ptr<SDK::UIControl> control) {
		control->updatePos();
	});
}
