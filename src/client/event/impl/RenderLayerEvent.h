#pragma once
#include "api/eventing/Event.h"
#include "util/FNV32.h"
#include "sdk/common/client/gui/ScreenView.h"
#include "sdk/common/client/renderer/screen/MinecraftUIRenderContext.h"

class RenderLayerEvent : public Event {
public:
	static const uint32_t hash = TOHASH(RenderLayerEvent);

	RenderLayerEvent(SDK::ScreenView* view, SDK::MinecraftUIRenderContext* ctx) : view(view), ctx(ctx) {}

	[[nodiscard]] SDK::ScreenView* getScreenView() { return view; }
	[[nodiscard]] SDK::MinecraftUIRenderContext* getUIRenderContext() { return ctx; }
private:
	SDK::ScreenView* view;
	SDK::MinecraftUIRenderContext* ctx;
};