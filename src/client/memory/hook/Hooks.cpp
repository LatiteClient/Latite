#include "pch.h"
#include "Hooks.h"

#include "hooks/GeneralHooks.h"
#include "hooks/LevelRendererHooks.h"
#include "hooks/OptionHooks.h"
#include "hooks/DXHooks.h"
#include "hooks/MinecraftGameHooks.h"
#include "hooks/RenderControllerHooks.h"
#include "hooks/ScreenViewHooks.h"
#include "hooks/PacketHooks.h"
#include "MinHook.h"
#include <vhook/vtable_hook.h>

using namespace std::chrono_literals;


LatiteHooks::LatiteHooks() {}

LatiteHooks::~LatiteHooks() {
	MH_Uninitialize();
}

void LatiteHooks::enable() {
	MH_EnableHook(MH_ALL_HOOKS);
}

void LatiteHooks::disable() {
	MH_DisableHook(MH_ALL_HOOKS);
	vh::unhook_all();
}
