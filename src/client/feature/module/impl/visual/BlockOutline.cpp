#include "pch.h"
#include "BlockOutline.h"
#include <util/DrawUtil3D.h>
#include <sdk/common/client/renderer/MaterialPtr.h>
#include <sdk/common/world/level/HitResult.h>

BlockOutline::BlockOutline() : Module("BlockOutline", "Block Overlay", "Changes the overlay in blocks!", GAME) {
	addSetting("renderThrough", "Render Through", "Whether to render the outline through blocks or not", this->renderThrough);
	addSetting("transparent", "Transparent", "Whether or not to have a transparent overlay", this->transparent, "renderThrough"_isfalse);
	
	addSetting("vanillaOutline", "Vanilla Outline", "Keep the vanilla outline.", this->outlineVanilla);
	addSetting("outline", "Outline", "Render an outline", this->outline, "vanillaOutline"_isfalse);
	addSetting("outlineColor", "Outline Color", "The outline color", this->outlineColor, "outline");

	addSetting("overlay", "Overlay", "Render an overlay", this->overlay);
	addSetting("faceOverlay", "Face Overlay", "Render the selected face of the block.", this->faceOverlay);
	addSetting("overlayColor", "Overlay Color", "The overlay color", this->overlayColor, "overlay");
	
	listen<OutlineSelectionEvent>((EventListenerFunc)&BlockOutline::onOutlineSelection, false,1);
}

void BlockOutline::onOutlineSelection(Event& evG) {
	auto& ev = reinterpret_cast<OutlineSelectionEvent&>(evG);
	if (!std::get<BoolValue>(outlineVanilla))
		ev.setCancelled();

	auto hitResult = SDK::ClientInstance::get()->minecraft->getLevel()->getHitResult();

	auto lrp = SDK::ClientInstance::get()->levelRenderer->getLevelRendererPlayer();
	MCDrawUtil3D dc{ SDK::ClientInstance::get()->levelRenderer, SDK::ScreenContext::instance3d,
		std::get<BoolValue>(renderThrough) ? SDK::MaterialPtr::getUIColor() : (std::get<BoolValue>(transparent) ? lrp->getSelectionOverlayMaterial() : lrp->getSelectionBoxMaterial())};

	if (std::get<BoolValue>(outline)) {
		dc.drawBox(ev.getBoundingBox(), std::get<ColorValue>(outlineColor).color1);
	}

	auto outlineCol = std::get<ColorValue>(outlineColor).color1;
	auto overlayCol = std::get<ColorValue>(overlayColor).color1;

	if (std::get<BoolValue>(outline)) {
		dc.drawBox(ev.getBoundingBox(), outlineCol);
		dc.flush();
	}

	if (std::get<BoolValue>(overlay)) {
		auto bp = Vec3{ (float)ev.getBlockPos().x, (float)ev.getBlockPos().y, (float)ev.getBlockPos().z };
		auto blockFace = hitResult->face;

		auto drawUp = [&]() {
			dc.fillQuad({ bp.x, bp.y + 1.f, bp.z }, { bp.x + 1.f, bp.y + 1.f, bp.z }, { bp.x + 1.f, bp.y + 1.f, bp.z + 1.f }, { bp.x, bp.y + 1.f, bp.z + 1.f }, overlayCol);
		};

		auto drawDown = [&]() {
			dc.fillQuad(bp, { bp.x + 1.f, bp.y, bp.z }, { bp.x + 1.f, bp.y, bp.z + 1.f }, { bp.x, bp.y, bp.z + 1.f }, overlayCol);
		};

		auto drawEast = [&]() {
			dc.fillQuad({ bp.x + 1.f, bp.y, bp.z }, { bp.x + 1.f, bp.y + 1.f, bp.z }, { bp.x + 1.f, bp.y + 1.f, bp.z + 1.f }, {bp.x + 1.f, bp.y, bp.z + 1.f}, overlayCol);
		};

		auto drawWest = [&]() {
			dc.fillQuad({ bp.x, bp.y, bp.z }, { bp.x, bp.y + 1.f, bp.z }, { bp.x, bp.y + 1.f, bp.z + 1.f }, { bp.x, bp.y, bp.z + 1.f }, overlayCol);
		};

		auto drawSouth = [&]() {
			dc.fillQuad({ bp.x, bp.y, bp.z + 1.f }, { bp.x, bp.y + 1.f, bp.z + 1.f }, { bp.x + 1.f, bp.y + 1.f, bp.z + 1.f }, { bp.x + 1.f, bp.y, bp.z + 1.f }, overlayCol);
		};

		auto drawNorth = [&]() {
			dc.fillQuad({ bp.x, bp.y, bp.z }, { bp.x, bp.y + 1.f, bp.z }, { bp.x + 1.f, bp.y + 1.f, bp.z }, { bp.x + 1.f, bp.y, bp.z }, overlayCol);
		};

		/*
		DOWN = 0,
	UP = 1,
	NORTH = 2,
	SOUTH = 3,
	WEST = 4,
	EAST = 5*/

		if (std::get<BoolValue>(faceOverlay)) {
			switch (blockFace) {
			case 0:
				drawDown();
				break;
			case 1: // up
				drawUp();
				break;
			case 2: // north
				drawNorth();
				break;
			case 3:
				drawSouth();
				break;
			case 4:
				drawWest();
				break;
			case 5:
				drawEast();
				break;
			}
		}
		else {
			drawNorth();
			drawEast();
			drawSouth();
			drawWest();
			drawUp();
			drawDown();
		}

		dc.flush();
	}
}
