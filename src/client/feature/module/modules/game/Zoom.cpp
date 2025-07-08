#include "pch.h"
#include "Zoom.h"
#include "client/Latite.h"
#include "client/render/Renderer.h"

Zoom::Zoom() : Module("Zoom", LocalizeString::get("client.module.zoom.name"),
                      LocalizeString::get("client.module.zoom.desc"), GAME, nokeybind) {
    addSetting("zoomKey", LocalizeString::get("client.module.zoom.zoomKey.name"),
               LocalizeString::get("client.module.zoom.zoomKey.desc"), this->zoomKey);
    addSliderSetting("modifier", LocalizeString::get("client.module.zoom.modifier.name"),
                     LocalizeString::get("client.module.zoom.modifier.desc"), this->modifier, FloatValue(1.f),
                     FloatValue(50.f), FloatValue(1.f));
    addSetting("animation", LocalizeString::get("client.module.zoom.animation.name"),
               LocalizeString::get("client.module.zoom.animation.desc"), hasAnim);
    addSliderSetting("animationSpeed", LocalizeString::get("client.module.zoom.animationSpeed.name"),
                     LocalizeString::get("client.module.zoom.animationSpeed.desc"), animSpeed, FloatValue(1.f),
                     FloatValue(5.f), FloatValue(1.f), "animation"_istrue);
    addSetting("cinematic", LocalizeString::get("client.module.zoom.cinematic.name"),
               LocalizeString::get("client.module.zoom.cinematic.desc"), this->cinematicCam);
    addSetting("hideHand", LocalizeString::get("client.module.zoom.hideHand.name"),
               LocalizeString::get("client.module.zoom.hideHand.desc"), this->hideHand);
    addSetting("dpiAdjust", LocalizeString::get("client.module.zoom.dpiAdjust.name"),
               LocalizeString::get("client.module.zoom.dpiAdjust.desc"), this->dpiAdjust);

    listen<RenderLevelEvent>(static_cast<EventListenerFunc>(&Zoom::onRenderLevel));
    listen<KeyUpdateEvent>(static_cast<EventListenerFunc>(&Zoom::onKeyUpdate));
    listen<ClickEvent>(static_cast<EventListenerFunc>(&Zoom::onClickUpdate));
    listen<CinematicCameraEvent>(static_cast<EventListenerFunc>(&Zoom::onCinematicCamera));
    listen<HideHandEvent>(static_cast<EventListenerFunc>(&Zoom::onHideHand));
    listen<SensitivityEvent>(static_cast<EventListenerFunc>(&Zoom::onSensitivity));
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