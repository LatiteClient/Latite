#include "GeneralHooks.h"
#include "sdk/common/client/game/ClientInstance.h"
#include "sdk/common/world/Minecraft.h"
#include "client/Latite.h"
#include "client/misc/ClientMessageSink.h"
#include "client/command/commandmanager.h"

#include "api/memory/memory.h"

std::shared_ptr<Hook> Level_tickHook;
std::shared_ptr<Hook> ChatScreenController_sendChatMesageHook;

void GenericHooks::Level_tick(sdk::Level* level)
{
	if (level == sdk::ClientInstance::get()->minecraft->getLevel()) {
		// Clientside level
		// dispatch clientside tick event..
		Latite::get().getClientMessageSink().doPrint(100);
	}
	return Level_tickHook->oFunc<decltype(&Level_tick)>()(level);
}

void GenericHooks::ChatScreenController_sendChatMessage(void* controller, std::string message)
{
	if (message.starts_with(CommandManager::prefix)) {
		Latite::get().getCommandManager().runCommand(message);
		return;
	}
	return ChatScreenController_sendChatMesageHook->oFunc<decltype(&ChatScreenController_sendChatMessage)>()(controller, message);
}

GenericHooks::GenericHooks() : HookGroup("General") {
	Level_tickHook = addHook(util::findSignature("48 89 5c 24 ? 48 89 74 24 ? 55 57 41 54 41 56 41 57 48 8b ec 48 83 ec ? 48 8b 05 ? ? ? ? 48 33 c4 48 89 45 ? 48 8b f9"),
		Level_tick, "Level::tick");

	ChatScreenController_sendChatMesageHook = addHook(memory::instructionToAddress(util::findSignature("e8 ? ? ? ? 3c ? 75 ? 48 8b 8f"), 1),
		ChatScreenController_sendChatMessage, "ChatScreenController::sendChatMessage");
}
