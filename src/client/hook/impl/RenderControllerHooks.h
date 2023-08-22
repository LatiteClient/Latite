/* Copyright (C) Imrglop, All rights reserved
 * Copyright (C) Latite Client contributors, All rights reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited without the permission of the author.
 * Proprietary and confidential
 * Written by Imrglop <contact: latiteclientgithub@gmail.com>, 2023
 */

#pragma once
#include "../Hooks.h"
#include "util/Util.h"

class RenderControllerHooks : public HookGroup {
	static Color* __fastcall getOverlayColor(void* thisptr, Color* out, void* ent);
public:
	RenderControllerHooks();
};