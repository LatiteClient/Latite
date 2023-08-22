/* Copyright (C) Imrglop, All rights reserved
 * Copyright (C) Latite Client contributors, All rights reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited without the permission of the author.
 * Proprietary and confidential
 * Written by Imrglop <contact: latiteclientgithub@gmail.com>, 2023
 */

#pragma once
#include "../Hooks.h"
#include "sdk/deps/Application/AppPlatform.h"

class AppPlatformHooks : public HookGroup {
	static int __fastcall _fireAppFocusLost(SDK::AppPlatform* plat);
public:
	AppPlatformHooks();
};