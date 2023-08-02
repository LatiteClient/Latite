#pragma once
#include "../Hook.h"

class OptionHooks : public HookGroup {
private:
	static float __fastcall Options_getGamma(void* options);
public:
	OptionHooks();
};