#include "pch.h"
#include <cmath>
#include "Coordinates.h"
#include <util/DxContext.h>

Coordinates::Coordinates() : HUDModule("Coordinates", "Coordinates", "Shows player position and other info", HUD) {
	addSetting("showDimension", "Show Dimension", "Show the dimension the player is currently in", this->showDimension);
}

void Coordinates::render(DrawUtil& dc, bool isDefault, bool inEditor) {
	// todo: fix module looking weird in mod menu dropdown (rect issue)
	// todo: option to hide vanilla coordinate display

	if (!SDK::ClientInstance::get()->getLocalPlayer()) return;
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
