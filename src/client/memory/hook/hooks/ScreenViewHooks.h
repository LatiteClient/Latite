#pragma once
#include "../Hook.h"
#include "mc/common/client/gui/ScreenView.h"

class ScreenViewHooks : public HookGroup {
	static void __fastcall setupAndRender(SDK::ScreenView* view, void* ctx);
public:
	ScreenViewHooks();
};