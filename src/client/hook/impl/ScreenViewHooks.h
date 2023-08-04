#pragma once
#include "../Hooks.h"
#include "sdk/common/client/gui/ScreenView.h"

class ScreenViewHooks : public HookGroup {
	static void __fastcall setupAndRender(sdk::ScreenView* view, void* ctx);
public:
	ScreenViewHooks();
};