#include "GeneralHooks.h"
#include "sdk/common/client/game/ClientInstance.h"
#include "sdk/common/world/Minecraft.h"
#include "sdk/signature/storage.h"
#include "client/Latite.h"
#include "client/misc/ClientMessageSink.h"
#include "client/feature/command/commandmanager.h"
#include "client/event/Eventing.h"
#include "client/event/impl/TickEvent.h"
#include "client/event/impl/RenderGameEvent.h"
#include "client/event/impl/KeyUpdateEvent.h"
#include "client/event/impl/ChatEvent.h"
#include "client/event/impl/ClickEvent.h"

#include "api/memory/memory.h"
#include "util/Logger.h"

namespace {
	std::shared_ptr<Hook> Level_tickHook;
	std::shared_ptr<Hook> ChatScreenController_sendChatMesageHook;
	std::shared_ptr<Hook> GameRenderer_renderCurrentFrameHook;
	std::shared_ptr<Hook> Keyboard_feedHook;
	std::shared_ptr<Hook> OnClickHook;
	std::shared_ptr<Hook> LoadLibraryHook;
}

void GenericHooks::Level_tick(sdk::Level* level) {
	if (level == sdk::ClientInstance::get()->minecraft->getLevel()) {
		// Clientside level
		// dispatch clientside tick event..
		TickEvent ev(level);
		Latite::get().getEventing().dispatchEvent(ev);
		Latite::get().getClientMessageSink().doPrint(100);
	}

	return Level_tickHook->oFunc<decltype(&Level_tick)>()(level);
}

void* GenericHooks::ChatScreenController_sendChatMessage(void* controller, std::string& message) {
	if (message.starts_with(CommandManager::prefix)) {
		Latite::get().getCommandManager().runCommand(message);
		return 0;
	}

	ChatEvent ev{ message };
	if (Eventing::get().dispatchEvent(ev)) return nullptr;

	return ChatScreenController_sendChatMesageHook->oFunc<decltype(&ChatScreenController_sendChatMessage)>()(controller, message);
}

void* GenericHooks::GameRenderer_renderCurrentFrame(void* rend) {
	RenderGameEvent ev{};
	Eventing::get().dispatchEvent(ev);
	return GameRenderer_renderCurrentFrameHook->oFunc<decltype(&GameRenderer_renderCurrentFrame)>()(rend);
}

void GenericHooks::Keyboard_feed(int key, bool isDown) {
	KeyUpdateEvent ev{key, isDown};
	if (Eventing::get().dispatchEvent(ev)) return;

	return Keyboard_feedHook->oFunc<decltype(&Keyboard_feed)>()(key, isDown);
}

void GenericHooks::onClick(ClickMap* map, char clickType, char isDownWheelDelta, uintptr_t a4, int16_t a5, int16_t a6, int16_t a7, char a8) {
	ClickEvent ev{ clickType, isDownWheelDelta };
	if (Eventing::get().dispatchEvent(ev)) return;
	return OnClickHook->oFunc<decltype(&onClick)>()(map, clickType, isDownWheelDelta, a4, a5, a6, a7, a8);
}

BOOL __stdcall GenericHooks::hkLoadLibraryW(LPCWSTR lib) {
	// prevent double injections
	return 0;
}

GenericHooks::GenericHooks() : HookGroup("General") {
	//LoadLibraryHook = addHook((uintptr_t)&::LoadLibraryW, hkLoadLibraryW);
	//LoadLibraryHook = addHook((uintptr_t) & ::LoadLibraryA, hkLoadLibraryW);


	Level_tickHook = addHook(Signatures::Level_tick.result,
		Level_tick, "Level::tick");

	ChatScreenController_sendChatMesageHook = addHook(Signatures::ChatScreenController_sendChatMessage.result,
		ChatScreenController_sendChatMessage, "ChatScreenController::sendChatMessage");

	GameRenderer_renderCurrentFrameHook = addHook(Signatures::GameRenderer__renderCurrentFrame.result,
		GameRenderer_renderCurrentFrame, "GameRenderer::_renderCurrentFrame");

	Keyboard_feedHook = addHook(Signatures::Keyboard_feed.result, Keyboard_feed, "Keyboard::feed");

	OnClickHook = addHook(Signatures::onClick.result, onClick, "onClick");

}
