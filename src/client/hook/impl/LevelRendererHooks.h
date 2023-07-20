#pragma once
#include "../Hooks.h"
#include "sdk/common/client/renderer/LevelRenderer.h"

class LevelRendererHooks : public HookGroup {
	static void* LevelRenderer_renderLevel(sdk::LevelRenderer* lvl, void* scn, void* unk);
public:
	LevelRendererHooks();
};