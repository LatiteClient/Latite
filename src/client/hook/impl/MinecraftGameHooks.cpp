#include "pch.h"
#include "MinecraftGameHooks.h"
#include "client/event/impl/AppSuspendedEvent.h"
#include "client/event/impl/UpdateEvent.h"
#include "client/event/Eventing.h"
#include "client/script/PluginManager.h"
#include "client/Latite.h"

namespace {
	std::shared_ptr<Hook> onAppSuspendedHook;
	std::shared_ptr<Hook> _updateHook;
}

void* MinecraftGameHooks::onAppSuspended(SDK::MinecraftGame* game,void*a,void*b,void*c) {
	AppSuspendedEvent ev{};
	Eventing::get().dispatch(ev);
	{
		PluginManager::Event sev{L"app-suspended", {}, false};
		Latite::getPluginManager().dispatchEvent(sev);
	}

	return onAppSuspendedHook->oFunc<decltype(&onAppSuspended)>()(game,a,b,c);
}

void __fastcall MinecraftGameHooks::_update(SDK::MinecraftGame* game) {
	_updateHook->oFunc<decltype(&_update)>()(game);
	UpdateEvent ev{};
	Eventing::get().dispatch(ev);

	{
		PluginManager::Event sev{L"renderDX", {}, false};
		Latite::getPluginManager().dispatchEvent(sev);
	}
}

MinecraftGameHooks::MinecraftGameHooks() {
	onAppSuspendedHook = addHook(Signatures::MinecraftGame_onAppSuspended.result, onAppSuspended,
		"MinecraftGame::onAppSuspended");
	_updateHook = addHook(Signatures::MinecraftGame__update.result, _update,
		"MinecraftGame::_update");
}
