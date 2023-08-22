/* Copyright (C) Imrglop, All rights reserved
 * Copyright (C) Latite Client contributors, All rights reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited without the permission of the author.
 * Proprietary and confidential
 * Written by Imrglop <contact: latiteclientgithub@gmail.com>, 2023
 */

#pragma once
#include "../Hooks.h"
#include "sdk/common/client/game/MinecraftGame.h"

class MinecraftGameHooks : public HookGroup {
	static void* __fastcall onAppSuspended(SDK::MinecraftGame* game);
	static void* __fastcall _update(SDK::MinecraftGame* game);
public:
	MinecraftGameHooks();
};