#include "pch.h"
#include "ScreenViewHooks.h"
#include "client/event/Eventing.h"
#include "client/event/impl/RenderLayerEvent.h"
#include "client/event/impl/RenderGameEvent.h"
#include "sdk/common/client/gui/controls/VisualTree.h"
#include "sdk/common/client/gui/controls/UIControl.h"

namespace {
	std::shared_ptr<Hook> setupAndRenderHook;
}

void __fastcall ScreenViewHooks::setupAndRender(SDK::ScreenView* view, void* ctx) {
	setupAndRenderHook->oFunc<decltype(&setupAndRender)>()(view, ctx);
	RenderLayerEvent ev{ view, reinterpret_cast<SDK::MinecraftUIRenderContext*>(ctx) };
	Eventing::get().dispatch(ev);

	RenderGameEvent evt{ };
	Eventing::get().dispatch(evt);
}

ScreenViewHooks::ScreenViewHooks() : HookGroup("ScreenView") {
	setupAndRenderHook = addHook(Signatures::ScreenView_setupAndRender.result, setupAndRender,
		"ScreenView::setupAndRender");
}
