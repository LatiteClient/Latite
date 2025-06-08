#pragma once
#include "client/event/Eventing.h"
#include "client/event/impl/RenderOverlayEvent.h"
#include "client/event/impl/RendererCleanupEvent.h"
#include "../../Module.h"
#include <vector>

class MotionBlur : public Module {
public:
	MotionBlur();
	~MotionBlur() = default;

	void onEnable() override;
	void onDisable() override;

	void onRenderOverlay(Event& genericEv);
	void onCleanup(Event& genericEv);
	void onRendererInit(Event& genericEv);

private:
	void clearFrames();

	// For "Pixel average" mode
	std::vector<ID2D1Bitmap1*> m_frameHistory;

	// For "Single frame" mode
	ID2D1Bitmap1* m_previousFrameBitmap = nullptr;

	bool m_lastModeWasPixelAverage = false;

	ValueType usePixelAverage = BoolValue(true);
	ValueType intensity = FloatValue(5.f);
	ValueType opacity = FloatValue(5.f);
};