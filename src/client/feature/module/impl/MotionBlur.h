#pragma once
#include "client/event/Eventing.h"
#include "client/event/impl/RenderOverlayEvent.h"
#include "client/event/impl/RendererCleanupEvent.h"
#include "../Module.h"

class MotionBlur : public Module {
public:
	MotionBlur();
	~MotionBlur() = default;

	void onEnable() override;
	void onDisable() override;

	void onRender(Event& genericEv);
	void onCleanup(Event& genericEv);
private:
	ID2D1Bitmap1* mbBitmap = nullptr;
};