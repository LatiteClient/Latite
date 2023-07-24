#pragma once
#include "../Hooks.h"
#include "sdk/deps/Application/AppPlatform.h"

class AppPlatformHooks : public HookGroup {
	static int _fireAppFocusLost(sdk::AppPlatform* plat);
public:
	AppPlatformHooks();
};