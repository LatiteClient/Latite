#include "Zoom.h"
#include "sdk/common/client/game/ClientInstance.h"
#include "sdk/common/world/Minecraft.h"
#include "client/Latite.h"
#include "client/render/Renderer.h"

Zoom::Zoom() : Module("Zoom", "Zoom", "Zooms like OptiFine", GAME, nokeybind) {
	addSetting("zoomKey", "Zoom Key", "The key to press to zoom", this->zoomKey);
	addSliderSetting("modifier", "Modifier", "How far to zoom", this->modifier, FloatValue(1.f), FloatValue(50.f), FloatValue(1.f));
	addSliderSetting("animationSpeed", "Speed", "The speed of the animation", animSpeed, FloatValue(1.f), FloatValue(50.f), FloatValue(1.f));

	listen<RenderLevelEvent>((EventListenerFunc)&Zoom::onRenderLevel);
	listen<KeyUpdateEvent>((EventListenerFunc)&Zoom::onKeyUpdate);
}

void Zoom::onRenderLevel(Event& evGeneric) {
	auto& ev = reinterpret_cast<RenderLevelEvent&>(evGeneric);

	modifyTo = shouldZoom ? std::get<FloatValue>(modifier).value : 1.f;

	// partial ticks
	float alpha = Latite::getRenderer().getDeltaTime();
	float lr = std::lerp(activeModifier, modifyTo, alpha * std::get<FloatValue>(animSpeed) / 10.f);
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
