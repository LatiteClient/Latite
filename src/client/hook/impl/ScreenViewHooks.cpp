/* Copyright (C) Imrglop, All rights reserved
 * Copyright (C) Latite Client contributors, All rights reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited without the permission of the author.
 * Proprietary and confidential
 * Written by Imrglop <contact: latiteclientgithub@gmail.com>, 2023
 */

#include "ScreenViewHooks.h"
#include "sdk/signature/storage.h"
#include "client/event/Eventing.h"
#include "client/event/impl/RenderLayerEvent.h"
#include "client/event/impl/RenderGameEvent.h"

namespace {
	std::shared_ptr<Hook> setupAndRenderHook;
}

void __fastcall ScreenViewHooks::setupAndRender(SDK::ScreenView* view, void* ctx) {
	setupAndRenderHook->oFunc<decltype(&setupAndRender)>()(view, ctx);
	RenderLayerEvent ev{ view };
	Eventing::get().dispatch(ev);

	RenderGameEvent evt{ };
	Eventing::get().dispatch(evt);
}

ScreenViewHooks::ScreenViewHooks() : HookGroup("ScreenView") {
	setupAndRenderHook = addHook(Signatures::ScreenView_setupAndRender.result, setupAndRender,
		"ScreenView::setupAndRender");
}
