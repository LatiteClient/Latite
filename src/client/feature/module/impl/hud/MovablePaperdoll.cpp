#include "pch.h"
#include "MovablePaperdoll.h"
#include <sdk/common/client/gui/ScreenView.h>
#include <sdk/common/client/gui/controls/VisualTree.h>
#include <sdk/common/client/gui/controls/UIControl.h>
#include <sdk/common/client/renderer/HudPlayerRenderer.h>

MovablePaperdoll::MovablePaperdoll() : HUDModule("Paperdoll", "Movable Paperdoll", "Be able to move around the paper doll!", HUD) {
	addSetting("alwaysShow", "Always Show", "Always show the paper doll.", alwaysShow);

	listen<RenderLayerEvent>((EventListenerFunc)&MovablePaperdoll::onRenderLayer, true, 10 /*need to overpower the hud renderer*/);
}

void MovablePaperdoll::render(DrawUtil& ctx, bool isDefault, bool inEditor) {
	auto lp = SDK::ClientInstance::get()->getLocalPlayer();
	if (!lp) return;

	float guiScale = SDK::ClientInstance::get()->getGuiData()->guiScale;

	if (hudPlayer) {
		hudPlayer->position = { rect.left / guiScale, rect.top / guiScale };
		hudPlayer->bounds = { getScale() * 15.f, getScale() * 15.f };
		this->rect.right = rect.left + 15.f * guiScale;
		this->rect.bottom = rect.top + 30.f * guiScale;

		if (std::get<BoolValue>(alwaysShow)) {
			auto comp = (SDK::CustomRenderComponent*)hudPlayer->uiComponents[4];
			if (auto rnd = comp->rend) {
				if (rnd) {
					rnd->timeToClose = 1.f;
				}
			}
		}
	}
	else {
		this->rect.right = rect.left + 15.f * guiScale;
		this->rect.bottom = rect.top + 30.f * guiScale;
	}
}

void MovablePaperdoll::onRenderLayer(Event& evG) {
	auto& ev = reinterpret_cast<RenderLayerEvent&>(evG);
	auto lp = SDK::ClientInstance::get()->getLocalPlayer();

	if (!lp) {
		this->hudPlayer = nullptr;
		return;
	}

	if (!hudPlayer && this->isActive() && this->isEnabled()) {
		if (ev.getScreenView()->visualTree->rootControl->name == XOR_STRING("hud_screen")) {
			this->hudPlayer = ev.getScreenView()->visualTree->rootControl->findFirstDescendantWithName(XOR_STRING("hud_player"));
		}
	}
}
