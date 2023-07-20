#include "Zoom.h"
#include "sdk/common/client/game/ClientInstance.h"
#include "sdk/common/world/Minecraft.h"

Zoom::Zoom() : Module("Zoom", "Zoom", "Zooms like OptiFine") {
	addSetting("modifier", "Modifier", "How far to zoom", this->modifier);
	addSetting("zoomKey", "Zoom Key", "The key to press to zoom", this->zoomKey);
	listen<RenderLevelEvent>((EventListenerFunc)&Zoom::onRenderLevel);
	listen<KeyUpdateEvent>((EventListenerFunc)&Zoom::onKeyUpdate);
}

void Zoom::onRenderLevel(Event& evGeneric) {
	auto ev = reinterpret_cast<RenderLevelEvent&>(evGeneric);

	modifyTo = shouldZoom ? std::get<FloatValue>(modifier).value : 1.f;

	// partial ticks
	auto alpha = sdk::ClientInstance::get()->minecraft->timer->alpha;
	float lr = std::lerp(activeModifier, modifyTo, alpha * 0.2f);
	activeModifier = lr;

	float& fx = ev.getLevelRenderer()->levelRendererPlayer->fovX;
	float& fy = ev.getLevelRenderer()->levelRendererPlayer->fovY;

	fx *= activeModifier;
	fy *= activeModifier;
}

void Zoom::onKeyUpdate(Event& evGeneric) {
	auto ev = reinterpret_cast<KeyUpdateEvent&>(evGeneric);
	if (ev.getKey() == std::get<KeyValue>(this->zoomKey)) {
		this->shouldZoom = ev.isDown();
	}
}
