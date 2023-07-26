#include "MinecraftGameHooks.h"
#include "sdk/signature/storage.h"
#include "client/event/impl/AppSuspendedEvent.h"
#include "client/event/Eventing.h"

namespace {
	std::shared_ptr<Hook> onAppSuspendedHook;
}

void* MinecraftGameHooks::onAppSuspended(sdk::MinecraftGame* game) {
	AppSuspendedEvent ev{};
	Eventing::get().dispatchEvent(ev);
	return onAppSuspendedHook->oFunc<decltype(&onAppSuspended)>()(game);
}

MinecraftGameHooks::MinecraftGameHooks() {
	onAppSuspendedHook = addHook(Signatures::MinecraftGame_onAppSuspended.result, onAppSuspended,
		"MinecraftGame::onAppSuspended");
}
