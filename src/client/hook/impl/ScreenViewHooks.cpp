#include "ScreenViewHooks.h"
#include "sdk/signature/storage.h"
#include "client/event/Eventing.h"
#include "client/event/impl/RenderLayerEvent.h"
#include "client/event/impl/RenderGameEvent.h"

namespace {
	std::shared_ptr<Hook> setupAndRenderHook;
}

void __fastcall ScreenViewHooks::setupAndRender(sdk::ScreenView* view, void* ctx) {
	setupAndRenderHook->oFunc<decltype(&setupAndRender)>()(view, ctx);
	RenderLayerEvent ev{ view };
	Eventing::get().dispatchEvent(ev);

	RenderGameEvent evt{ };
	Eventing::get().dispatchEvent(evt);
}

ScreenViewHooks::ScreenViewHooks() : HookGroup("ScreenView") {
	setupAndRenderHook = addHook(Signatures::ScreenView_setupAndRender.result, setupAndRender,
		"ScreenView::setupAndRender");
}
