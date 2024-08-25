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

	// Maybe enable hooks manually instead of using "MH_EnableHook(MH_ALL_HOOKS)" and use a parameter for a custom callback if a hook is enabled/disabled
	static auto rotateAddr = reinterpret_cast<void*>(Signatures::glm_rotateRef.result);
	// glm::rotate hook in ItemRenderer::render
	// Replaces 0xE9 with 0xE8 so a "CALL" instruction is used instead of a "JMP" instruction as we don't want to overwrite the other code
	memory::patchBytes(rotateAddr, (BYTE*)"\xE8", 1);
}

void LatiteHooks::disable() {
	MH_DisableHook(MH_ALL_HOOKS);
	vh::unhook_all();
}
