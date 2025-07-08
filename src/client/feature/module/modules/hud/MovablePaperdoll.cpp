#include "pch.h"
#include "MovablePaperdoll.h"
#include <mc/common/client/gui/ScreenView.h>
#include <mc/common/client/gui/controls/VisualTree.h>
#include <mc/common/client/gui/controls/UIControl.h>
#include <mc/common/client/renderer/HudPlayerRenderer.h>

MovablePaperdoll::MovablePaperdoll() : HUDModule("Paperdoll",
                                                 LocalizeString::get("client.hudmodule.movablePaperdoll.name"),
                                                 LocalizeString::get("client.hudmodule.movablePaperdoll.desc"), HUD) {
    addSetting("alwaysShow", LocalizeString::get("client.hudmodule.movablePaperdoll.alwaysShow.name"),
               LocalizeString::get("client.hudmodule.movablePaperdoll.alwaysShow.desc"), alwaysShow);

    listen<RenderLayerEvent>(static_cast<EventListenerFunc>(&MovablePaperdoll::onRenderLayer), true,
                             10 /*need to overpower the hud renderer*/);
}

void MovablePaperdoll::render(DrawUtil& ctx, bool, bool) {
}

void MovablePaperdoll::onRenderLayer(Event& evG) {
	auto& ev = reinterpret_cast<RenderLayerEvent&>(evG);
	auto lp = SDK::ClientInstance::get()->getLocalPlayer();

	if (!lp) {
		this->hudPlayer = nullptr;
		return;
	}

	if (this->isActive() && this->isEnabled()) {
		if (ev.getScreenView()->visualTree->rootControl->name == XOR_STRING("hud_screen")) {
			this->hudPlayer = ev.getScreenView()->visualTree->rootControl->findFirstDescendantWithName(XOR_STRING("hud_player"));

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
	}
}
