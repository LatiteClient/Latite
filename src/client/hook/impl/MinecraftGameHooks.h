#pragma once
#include "../Hooks.h"
#include "sdk/common/client/game/MinecraftGame.h"

class MinecraftGameHooks : public HookGroup {
	static void* __fastcall onAppSuspended(SDK::MinecraftGame* game,void*,void*,void*);
	static void* __fastcall _update(SDK::MinecraftGame* game);
public:
	MinecraftGameHooks();
};