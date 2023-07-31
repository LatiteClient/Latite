#pragma once
#include "../../Module.h"
#include "client/event/impl/RenderLevelEvent.h"
#include "client/event/impl/KeyUpdateEvent.h"
#include "api/feature/setting/Setting.h"

class Zoom : public Module {
public:

	Zoom();
	~Zoom() = default;

	void onRenderLevel(Event& ev);
	void onKeyUpdate(Event& ev);
private:
	Setting::Value zoomKey = KeyValue('C');
	Setting::Value modifier = FloatValue(2.f);
	Setting::Value animSpeed = FloatValue(2.f);
	float activeModifier = 1.f;
	float modifyTo = 1.f;
	bool shouldZoom = false;
};