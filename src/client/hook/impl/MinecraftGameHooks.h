#pragma once
#include "../Hooks.h"
#include "sdk/common/client/game/MinecraftGame.h"

class MinecraftGameHooks : public HookGroup {
	static void* onAppSuspended(sdk::MinecraftGame* game);
public:
	MinecraftGameHooks();
};