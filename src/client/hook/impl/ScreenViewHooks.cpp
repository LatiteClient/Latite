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

	//MCDrawUtil dc{ ev.getUIRenderContext(), SDK::ClientInstance::get()->minecraftGame->minecraftFont };
	//dc.fillRoundedRectangle({ 50.f, 50.f, 150.f, 150.f}, {1.f,1.f,1.f,1.f}, 19.f);
	//if (view->visualTree->rootControl->name == "hud_screen") dc.drawVignette({1.f, 0.f, 0.f, 0.4f}, 0.f);
	RenderGameEvent evt{ };
	Eventing::get().dispatch(evt);
}

ScreenViewHooks::ScreenViewHooks() : HookGroup("ScreenView") {
	setupAndRenderHook = addHook(Signatures::ScreenView_setupAndRender.result, setupAndRender,
		"ScreenView::setupAndRender");
}
