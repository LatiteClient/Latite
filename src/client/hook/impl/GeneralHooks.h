#pragma once
#include "client/hook/Hook.h"
#include "util/util.h"
#include "sdk/common/world/level/Level.h"

class GeneralHooks : public HookGroup {
	static void Level_tick(sdk::Level* level);
	static void ChatScreenController_sendChatMessage(void* controller, std::string message);
public:
	GeneralHooks();
};