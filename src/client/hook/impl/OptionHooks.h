#pragma once
#include "../Hook.h"

class OptionHooks : public HookGroup {
private:
	static float Options_getGamma(void* options);
public:
	OptionHooks();
};