#pragma once
#include "../../Module.h"
#include "client/event/events/RenderLevelEvent.h"
#include "client/event/events/KeyUpdateEvent.h"
#include "client/event/events/CinematicCameraEvent.h"
#include "client/feature/setting/Setting.h"

class Zoom : public Module {
public:

	Zoom();
	~Zoom() = default;

private:
	void onRenderLevel(Event& ev);
	void onKeyUpdate(Event& ev);
	void onClickUpdate(Event& ev);
	void onCinematicCamera(Event& ev);
	void onHideHand(Event& ev);
	void onSensitivity(Event& ev);
	
	ValueType zoomKey = KeyValue('C');
	ValueType modifier = FloatValue(15.f);
	ValueType hasAnim = BoolValue(false);
	ValueType animSpeed = FloatValue(2.f);
	ValueType cinematicCam = BoolValue(true);
	ValueType hideHand = BoolValue(true);
	ValueType dpiAdjust = BoolValue(true);

	float zoomModifier = 0.f;
	float activeModifier = 1.f;
	float modifyTo = 1.f;
	bool shouldZoom = false;
};