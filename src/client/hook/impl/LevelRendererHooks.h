#pragma once
#include "../Hook.h"
#include "sdk/common/client/renderer/game/LevelRenderer.h"

class LevelRendererHooks : public HookGroup {
	static void __fastcall LevelRenderer_renderLevel(SDK::LevelRenderer* lvl, SDK::ScreenContext* scn, void* unk);
public:
	LevelRendererHooks();
};