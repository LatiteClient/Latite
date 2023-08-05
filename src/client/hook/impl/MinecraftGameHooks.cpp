#include "MinecraftGameHooks.h"
#include "sdk/signature/storage.h"
#include "client/event/impl/AppSuspendedEvent.h"
#include "client/event/impl/UpdateEvent.h"
#include "client/event/Eventing.h"

namespace {
	std::shared_ptr<Hook> onAppSuspendedHook;
	std::shared_ptr<Hook> _updateHook;
}

void* MinecraftGameHooks::onAppSuspended(sdk::MinecraftGame* game) {
	AppSuspendedEvent ev{};
	Eventing::get().dispatchEvent(ev);
	return onAppSuspendedHook->oFunc<decltype(&onAppSuspended)>()(game);
}

void* __fastcall MinecraftGameHooks::_update(sdk::MinecraftGame* game) {
	auto ret = _updateHook->oFunc<decltype(&_update)>()(game);
	UpdateEvent ev{};
	Eventing::get().dispatchEvent(ev);
	return ret;
}

MinecraftGameHooks::MinecraftGameHooks() {
	onAppSuspendedHook = addHook(Signatures::MinecraftGame_onAppSuspended.result, onAppSuspended,
		"MinecraftGame::onAppSuspended");
	_updateHook = addHook(Signatures::MinecraftGame__update.result, _update,
		"MinecraftGame::_update");
}
