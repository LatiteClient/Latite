#include "pch.h"
#include "MovableScoreboard.h"

#include <mc/common/client/gui/ScreenView.h>
#include <mc/common/client/gui/controls/VisualTree.h>
#include <mc/common/client/gui/controls/UIControl.h>

MovableScoreboard::MovableScoreboard() : HUDModule("MovableScoreboard",
                                                   LocalizeString::get("client.module.movableScoreboard.name"),
                                                   LocalizeString::get("client.module.movableScoreboard.desc"), HUD, 0,
                                                   false) {
	listen<RenderLayerEvent>((EventListenerFunc)&MovableScoreboard::onRenderLayer, true, 10 /*need to overpower the hud renderer*/);
}

void MovableScoreboard::render(DrawUtil& ctx, bool isDefault, bool inEditor) {
}

void MovableScoreboard::onRenderLayer(Event& evG) {
	auto& ev = reinterpret_cast<RenderLayerEvent&>(evG);
	auto lp = SDK::ClientInstance::get()->getLocalPlayer();

	if (!lp) {
		this->sidebar = nullptr;
		return;
	}

	if (this->isActive() && this->isEnabled()) {
		if (ev.getScreenView()->visualTree->rootControl->name == XOR_STRING("hud_screen")) {
			this->sidebar = ev.getScreenView()->visualTree->rootControl->findFirstDescendantWithName(XOR_STRING("sidebar"));

			auto lp = SDK::ClientInstance::get()->getLocalPlayer();
			if (!lp) return;

			float guiScale = SDK::ClientInstance::get()->getGuiData()->guiScale;

			if (sidebar) {
				sidebar->position = { rect.left / guiScale, rect.top / guiScale };
				updatePos();

				this->rect.right = rect.left + sidebar->bounds.x * guiScale;
				this->rect.bottom = rect.top + sidebar->bounds.y * guiScale;
			}
			else {
				this->rect.right = rect.left + 30.f;
				this->rect.bottom = rect.top + 30.f;
			}
		}
	}
}

void MovableScoreboard::updatePos() {
	sidebar->getDescendants([](std::shared_ptr<SDK::UIControl> control) {
		control->updatePos();
		});
}
