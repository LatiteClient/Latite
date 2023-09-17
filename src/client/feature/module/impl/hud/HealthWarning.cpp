#include "pch.h"
#include "HealthWarning.h"
#include "client/event/impl/RenderLayerEvent.h"
#include "sdk/common/client/gui/controls/VisualTree.h"
#include "sdk/common/client/gui/controls/UIControl.h"

HealthWarning::HealthWarning() : Module("HealthWarning", "Health Warning", "Give the screen a vignette when your health is low", HUD, nokeybind) {
    this->listen<RenderLayerEvent>(&HealthWarning::onRenderLayer);
	addSetting("vignetteColor", "Vignette Color", "", vignetteColor);
}

void HealthWarning::onRenderLayer(Event& evG) {
	RenderLayerEvent& ev = reinterpret_cast<RenderLayerEvent&>(evG);
	SDK::ScreenView* screenView = ev.getScreenView();

	if (!SDK::ClientInstance::get()->getLocalPlayer()) return;

	StoredColor vignette = std::get<ColorValue>(vignetteColor).color1;

	MCDrawUtil dc{ ev.getUIRenderContext(), SDK::ClientInstance::get()->minecraftGame->minecraftFont };
	if (6.f > SDK::ClientInstance::get()->getLocalPlayer()->getHealth() && screenView->visualTree->rootControl->name == "hud_screen") {
		dc.drawVignette({ vignette.r, vignette.g, vignette.b, vignette.a }, 0);
	}
}
