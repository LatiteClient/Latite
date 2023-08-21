#pragma once
#include "../Hooks.h"
#include "sdk/common/client/renderer/LevelRenderer.h"

class LevelRendererHooks : public HookGroup {
	static void* __fastcall LevelRenderer_renderLevel(SDK::LevelRenderer* lvl, void* scn, void* unk);
public:
	LevelRendererHooks();
};