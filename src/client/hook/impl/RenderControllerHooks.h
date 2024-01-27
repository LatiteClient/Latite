#pragma once
#include "../Hook.h"
#include "util/Util.h"

class RenderControllerHooks : public HookGroup {
	static struct Color* __fastcall getOverlayColor(void* thisptr, Color* out, void* ent);
public:
	RenderControllerHooks();
};