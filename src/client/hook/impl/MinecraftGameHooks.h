#pragma once
#include "../Hooks.h"
#include "sdk/common/client/game/MinecraftGame.h"

class MinecraftGameHooks : public HookGroup {
	static void* __fastcall onAppSuspended(sdk::MinecraftGame* game);
	static void* __fastcall _update(sdk::MinecraftGame* game);
public:
	MinecraftGameHooks();
};