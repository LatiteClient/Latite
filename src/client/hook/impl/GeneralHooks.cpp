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

#include "api/memory/memory.h"

namespace {
	std::shared_ptr<Hook> Level_tickHook;
	std::shared_ptr<Hook> ChatScreenController_sendChatMesageHook;
	std::shared_ptr<Hook> GameRenderer_renderCurrentFrameHook;
	std::shared_ptr<Hook> Keyboard_feedHook;
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
	Eventing::get().dispatchEvent(ev);

	return ChatScreenController_sendChatMesageHook->oFunc<decltype(&ChatScreenController_sendChatMessage)>()(controller, message);
}

int GenericHooks::GameRenderer_renderCurrentFrame(void* rend) {
	RenderGameEvent ev{};
	Eventing::get().dispatchEvent(ev);
	return GameRenderer_renderCurrentFrameHook->oFunc<decltype(&GameRenderer_renderCurrentFrame)>()(rend);
}

void GenericHooks::Keyboard_feed(int key, bool isDown) {
	KeyUpdateEvent ev{key, isDown};
	Eventing::get().dispatchEvent(ev);
	return Keyboard_feedHook->oFunc<decltype(&Keyboard_feed)>()(key, isDown);
}

GenericHooks::GenericHooks() : HookGroup("General") {
	Level_tickHook = addHook(util::findSignature("48 89 5c 24 ? 48 89 74 24 ? 55 57 41 54 41 56 41 57 48 8b ec 48 83 ec ? 48 8b 05 ? ? ? ? 48 33 c4 48 89 45 ? 48 8b f9"),
		Level_tick, "Level::tick");

	ChatScreenController_sendChatMesageHook = addHook(memory::instructionToAddress(util::findSignature("e8 ? ? ? ? 3c ? 75 ? 48 8b 8f"), 1),
		ChatScreenController_sendChatMessage, "ChatScreenController::sendChatMessage");

	GameRenderer_renderCurrentFrameHook = addHook(memory::instructionToAddress(util::findSignature("e8 ? ? ? ? 90 48 8d 8d ? ? ? ? e8 ? ? ? ? 90 48 8d 8d ? ? ? ? e8 ? ? ? ? 48 8b 86"), 1),
		GameRenderer_renderCurrentFrame, "GameRenderer::_renderCurrentFrame");

	Keyboard_feedHook = addHook(Signatures::Keyboard_feed.result, Keyboard_feed, "Keyboard::feed");
}
