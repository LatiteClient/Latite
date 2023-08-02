#pragma once
#include "../Hooks.h"
#include "util/Util.h"

class RenderControllerHooks : public HookGroup {
	static Color* __fastcall getOverlayColor(void* thisptr, Color* out, void* ent);
public:
	RenderControllerHooks();
};