#pragma once
#include "../Hook.h"

class OptionHooks : public HookGroup {
private:
	static float Options_getGamma(void* options, void*);
	static int Options_getPerspective(void* options);
	static bool Options_getHideHand(void* options);
	static float Options_getSensitivity(void* options, unsigned int, int a3, int a4, int a5);
public:
	OptionHooks();
};