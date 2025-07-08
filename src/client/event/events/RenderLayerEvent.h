#pragma once
#include "client/event/Event.h"
#include "util/Crypto.h"
#include "mc/common/client/gui/ScreenView.h"
#include "mc/common/client/renderer/screen/MinecraftUIRenderContext.h"

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