#include "MinecraftGameHooks.h"
#include "sdk/signature/storage.h"
#include "client/event/impl/AppSuspendedEvent.h"
#include "client/event/impl/UpdateEvent.h"
#include "client/event/Eventing.h"
#include "client/script/ScriptManager.h"
#include "client/Latite.h"

namespace {
	std::shared_ptr<Hook> onAppSuspendedHook;
	std::shared_ptr<Hook> _updateHook;
}

void* MinecraftGameHooks::onAppSuspended(SDK::MinecraftGame* game) {
	AppSuspendedEvent ev{};
	Eventing::get().dispatch(ev);
	return onAppSuspendedHook->oFunc<decltype(&onAppSuspended)>()(game);
}

void* __fastcall MinecraftGameHooks::_update(SDK::MinecraftGame* game) {
	auto ret = _updateHook->oFunc<decltype(&_update)>()(game);
	UpdateEvent ev{};
	Eventing::get().dispatch(ev);

	{
		ScriptManager::Event sev{L"renderDX", {}, false};
		Latite::getScriptManager().dispatchEvent(sev);
	}
	return ret;
}

MinecraftGameHooks::MinecraftGameHooks() {
	onAppSuspendedHook = addHook(Signatures::MinecraftGame_onAppSuspended.result, onAppSuspended,
		"MinecraftGame::onAppSuspended");
	_updateHook = addHook(Signatures::MinecraftGame__update.result, _update,
		"MinecraftGame::_update");
}
