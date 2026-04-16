#include "pch.h"
#include "MinecraftGameHooks.h"
#include "client/event/events/AppSuspendedEvent.h"
#include "client/event/events/UpdateEvent.h"
#include "client/event/Eventing.h"
#include "client/script/PluginManager.h"
#include "client/Latite.h"

namespace {
	std::shared_ptr<Hook> onDeviceLostHook;
	std::shared_ptr<Hook> _updateHook;
}

void MinecraftGameHooks::onDeviceLost(SDK::MinecraftGame* game) {
	FocusLostEvent ev{};
	
	if (Eventing::get().dispatch(ev))
		return;
	
	onDeviceLostHook->oFunc<decltype(&onDeviceLost)>()(game);
}

void __fastcall MinecraftGameHooks::_update(SDK::MinecraftGame* game) {
	BEGIN_ERROR_HANDLER
	_updateHook->oFunc<decltype(&_update)>()(game);
	UpdateEvent ev{};

	{
		PluginManager::Event sev{L"renderDX", {}, false};
		Latite::getPluginManager().dispatchEvent(sev);
	}

	Eventing::get().dispatch(ev);
	END_ERROR_HANDLER
}

MinecraftGameHooks::MinecraftGameHooks() {
	onDeviceLostHook = addHook(Signatures::MinecraftGame_onDeviceLost.result, onDeviceLost,
		"MinecraftGame::onDeviceLost");
	_updateHook = addHook(Signatures::MinecraftGame__update.result, _update,
		"MinecraftGame::_update");
}
