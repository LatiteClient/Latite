#pragma once
#include "../Hooks.h"
#include "sdk/deps/Application/AppPlatform.h"

class AppPlatformHooks : public HookGroup {
	static int __fastcall _fireAppFocusLost(SDK::AppPlatform* plat);
public:
	AppPlatformHooks();
};