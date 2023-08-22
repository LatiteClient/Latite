/* Copyright (C) Imrglop, All rights reserved
 * Copyright (C) Latite Client contributors, All rights reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited without the permission of the author.
 * Proprietary and confidential
 * Written by Imrglop <contact: latiteclientgithub@gmail.com>, 2023
 */

#include "LevelRendererHooks.h"
#include "sdk/signature/storage.h"
#include "client/event/Eventing.h"
#include "client/event/impl/RenderLevelEvent.h"
#include "client/render/Renderer.h"
#include "client/Latite.h"

namespace {
	std::shared_ptr<Hook> renderLevelHook;
}

void* LevelRendererHooks::LevelRenderer_renderLevel(SDK::LevelRenderer* lvl, void* scn, void* unk) {
	auto res = renderLevelHook->oFunc<decltype(&LevelRenderer_renderLevel)>()(lvl, scn, unk);
	{
		RenderLevelEvent ev{ lvl };
		Eventing::get().dispatch(ev);
	}
	return res;
}

LevelRendererHooks::LevelRendererHooks() {
	renderLevelHook = addHook(Signatures::LevelRenderer_renderLevel.result, LevelRenderer_renderLevel,
		"LevelRenderer::renderLevel");
}
