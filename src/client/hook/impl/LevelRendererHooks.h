/* Copyright (C) Imrglop, All rights reserved
 * Copyright (C) Latite Client contributors, All rights reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited without the permission of the author.
 * Proprietary and confidential
 * Written by Imrglop <contact: latiteclientgithub@gmail.com>, 2023
 */

#pragma once
#include "../Hooks.h"
#include "sdk/common/client/renderer/game/LevelRenderer.h"

class LevelRendererHooks : public HookGroup {
	static void* __fastcall LevelRenderer_renderLevel(SDK::LevelRenderer* lvl, void* scn, void* unk);
public:
	LevelRendererHooks();
};