#include "pch.h"
#include "Hooks.h"

#include "impl/GeneralHooks.h"
#include "impl/LevelRendererHooks.h"
#include "impl/OptionHooks.h"
#include "impl/DXHooks.h"
#include "impl/AppPlatformHooks.h"
#include "impl/MinecraftGameHooks.h"
#include "impl/RenderControllerHooks.h"
#include "impl/ScreenViewHooks.h"
#include "impl/PacketHooks.h"
#include "MinHook.h"
#include <vhook/vtable_hook.h>

using namespace std::chrono_literals;

void LatiteHooks::uninit() {
	MH_Uninitialize();
}

void LatiteHooks::enable() {
	MH_EnableHook(MH_ALL_HOOKS);
}

void LatiteHooks::disable() {
	MH_DisableHook(MH_ALL_HOOKS);
	vh::unhook_all();
}
