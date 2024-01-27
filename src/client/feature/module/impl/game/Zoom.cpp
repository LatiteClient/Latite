#include "pch.h"
#include "Zoom.h"
#include "client/Latite.h"
#include "client/render/Renderer.h"

Zoom::Zoom() : Module("Zoom", "Zoom", "Zooms like OptiFine", GAME, nokeybind) {
	addSetting("zoomKey", "Zoom Key", "The key to press to zoom", this->zoomKey);
	addSliderSetting("modifier", "Modifier", "How far to zoom", this->modifier, FloatValue(1.f), FloatValue(50.f), FloatValue(1.f));
	addSetting("animation", "Animation", "Whether to have a zoom animation or not", hasAnim);
	addSliderSetting("animationSpeed", "Speed", "The speed of the animation", animSpeed, FloatValue(1.f), FloatValue(5.f), FloatValue(1.f), "animation"_istrue);
	addSetting("cinematic", "Cinematic Camera", "Enable cinematic camera while Zoom is on", this->cinematicCam);
	addSetting("hideHand", "Hide Hand", "Whether or not to hide the hand when zooming.", this->hideHand);
	addSetting("dpiAdjust", "Adjust DPI", "Adjust the sensitivity while zooming.", this->dpiAdjust);

	listen<RenderLevelEvent>((EventListenerFunc)&Zoom::onRenderLevel);
	listen<KeyUpdateEvent>((EventListenerFunc)&Zoom::onKeyUpdate);
	listen<ClickEvent>((EventListenerFunc)&Zoom::onClickUpdate);
	listen<CinematicCameraEvent>((EventListenerFunc)&Zoom::onCinematicCamera);
	listen<HideHandEvent>((EventListenerFunc)&Zoom::onHideHand);
	listen<SensitivityEvent>((EventListenerFunc)&Zoom::onSensitivity);
}

void Zoom::onRenderLevel(Event& evGeneric) {
	auto& ev = reinterpret_cast<RenderLevelEvent&>(evGeneric);
	if (!shouldZoom)
		zoomModifier = 0.f;

	modifyTo = std::clamp(shouldZoom ? std::get<FloatValue>(modifier).value + zoomModifier : 1.f, 1.f, 60.f);

	// partial ticks
	float alpha = Latite::getRenderer().getDeltaTime();
	float lr = modifyTo;
	if (std::get<BoolValue>(hasAnim)) lr = std::lerp(activeModifier, modifyTo, alpha * std::get<FloatValue>(animSpeed) / 10.f);
	activeModifier = lr;

	float& fx = ev.getLevelRenderer()->getLevelRendererPlayer()->getFovX();
	float& fy = ev.getLevelRenderer()->getLevelRendererPlayer()->getFovY();

	fx *= activeModifier;
	fy *= activeModifier;
}

void Zoom::onKeyUpdate(Event& evGeneric) {
	auto& ev = reinterpret_cast<KeyUpdateEvent&>(evGeneric);
	if (ev.inUI()) return;
	if (ev.getKey() == std::get<KeyValue>(this->zoomKey)) {
		this->shouldZoom = ev.isDown();
	}
}

void Zoom::onClickUpdate(Event& evGeneric) {
	auto& ev = reinterpret_cast<ClickEvent&>(evGeneric);

	if (ev.getMouseButton() == 4 /* scroll */ && this->shouldZoom) {
		// later half of this line clamps scrolling
		zoomModifier += static_cast<float>(ev.getWheelDelta()) < 0 ? -1 : 1;
		ev.setCancelled(true);
	}
}

void Zoom::onCinematicCamera(Event& evGeneric) {
	auto& ev = reinterpret_cast<CinematicCameraEvent&>(evGeneric);
	if (shouldZoom && std::get<BoolValue>(this->cinematicCam)) {
		ev.setValue(true);
	}
}

void Zoom::onHideHand(Event& evG) {
	auto& ev = reinterpret_cast<HideHandEvent&>(evG);
	if (shouldZoom && std::get<BoolValue>(this->hideHand)) {
		ev.getValue() = true;
	}
}

void Zoom::onSensitivity(Event& evG) {
	auto& ev = reinterpret_cast<SensitivityEvent&>(evG);
	if (shouldZoom && std::get<BoolValue>(this->dpiAdjust)) {
		ev.getValue() = std::min(ev.getValue(), ev.getValue() * (2 / std::get<FloatValue>(modifier)));
	}
}