#include "pch.h"
#include "HealthWarning.h"
#include "client/event/impl/RenderLayerEvent.h"
#include "sdk/common/client/gui/controls/VisualTree.h"
#include "sdk/common/client/gui/controls/UIControl.h"

HealthWarning::HealthWarning() : Module("HealthWarning", LocalizeString::get("client.hudmodule.healthWarning.name"), LocalizeString::get("client.hudmodule.healthWarning.desc"), HUD, nokeybind) {
	this->listen<RenderLayerEvent>(&HealthWarning::onRenderLayer);
	addSetting("vignetteColor", LocalizeString::get("client.hudmodule.healthWarning.vignetteColor.name"), L"", vignetteColor);
	addSliderSetting("healthPointThreshold", LocalizeString::get("client.hudmodule.healthWarning.healthPointThreshold.name"), L"", healthPointThreshold, FloatValue(1.f), FloatValue(19.f), FloatValue(.5f));
	addSliderSetting("vignetteFade", LocalizeString::get("client.hudmodule.healthWarning.vignetteFade.name"), L"", vignetteFade, FloatValue(0.f), FloatValue(1.f), FloatValue(.1f));
}


void HealthWarning::onRenderLayer(Event& evG) {
	RenderLayerEvent& ev = reinterpret_cast<RenderLayerEvent&>(evG);
	SDK::ScreenView* screenView = ev.getScreenView();

	if (!SDK::ClientInstance::get()->getLocalPlayer()) return;

	StoredColor vignette = std::get<ColorValue>(vignetteColor).color1;

	MCDrawUtil dc{ ev.getUIRenderContext(), SDK::ClientInstance::get()->minecraftGame->minecraftFont };
	if (std::get<FloatValue>(healthPointThreshold) > SDK::ClientInstance::get()->getLocalPlayer()->getHealth() && screenView->visualTree->rootControl->name == "hud_screen") {
		dc.drawVignette({ vignette.r, vignette.g, vignette.b, vignette.a }, std::get<FloatValue>(vignetteFade));
	}
}
