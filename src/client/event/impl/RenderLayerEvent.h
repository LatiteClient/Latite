#pragma once
#include "api/eventing/Event.h"
#include "util/FNV32.h"
#include "sdk/common/client/gui/ScreenView.h"

class RenderLayerEvent : public Event {
public:
	static const uint32_t hash = TOHASH(RenderLayerEvent);

	RenderLayerEvent(sdk::ScreenView* view) : view(view) {}

	[[nodiscard]] sdk::ScreenView* getScreenView() { return view; }
private:
	sdk::ScreenView* view;
};