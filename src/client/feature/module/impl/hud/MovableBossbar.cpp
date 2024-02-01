#include "pch.h"
#include "MovableBossbar.h"

#include <sdk/common/client/gui/ScreenView.h>
#include <sdk/common/client/gui/controls/VisualTree.h>
#include <sdk/common/client/gui/controls/UIControl.h>


MovableBossbar::MovableBossbar() : HUDModule("MovableBossbar", "Movable Bossbar", "Be able to the bossbar!", HUD, 0, false) {
	listen<RenderLayerEvent>((EventListenerFunc)&MovableBossbar::onRenderLayer, true, 10 /*need to overpower the hud renderer*/);
}

void MovableBossbar::render(DrawUtil& ctx, bool isDefault, bool inEditor) {
}

void MovableBossbar::onRenderLayer(Event& evG) {
	auto& ev = reinterpret_cast<RenderLayerEvent&>(evG);
	auto lp = SDK::ClientInstance::get()->getLocalPlayer();

	if (!lp) {
		this->bossHealthGrid = nullptr;
		return;
	}

	if (this->isActive() && this->isEnabled()) {
		if (ev.getScreenView()->visualTree->rootControl->name == XOR_STRING("hud_screen")) {
			this->bossHealthGrid = ev.getScreenView()->visualTree->rootControl->findFirstDescendantWithName(XOR_STRING("boss_health_grid"));

			auto lp = SDK::ClientInstance::get()->getLocalPlayer();
			if (!lp) return;

			float guiScale = SDK::ClientInstance::get()->getGuiData()->guiScale;

			if (bossHealthGrid) {
				bossHealthGrid->position = { rect.left / guiScale, rect.top / guiScale };
				updatePos();

				this->rect.right = rect.left + bossHealthGrid->bounds.x * guiScale;
				this->rect.bottom = rect.top + bossHealthGrid->bounds.y * guiScale;
			}
			else {
				this->rect.right = rect.left + 100.f;
				this->rect.bottom = rect.top + 100.f;
			}
		}
	}
}

void MovableBossbar::updatePos() {
	bossHealthGrid->getDescendants([](std::shared_ptr<SDK::UIControl> control) {
		control->updatePos();
		});
}
