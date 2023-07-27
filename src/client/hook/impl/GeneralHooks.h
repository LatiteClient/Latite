#pragma once
#include "client/hook/Hook.h"
#include "sdk/common/world/level/Level.h"
#include "util/util.h"
#include "sdk/misc/ClickMap.h"

class GenericHooks : public HookGroup {
    static void Level_tick(sdk::Level* level);
    static void* ChatScreenController_sendChatMessage(void* controller, std::string& message);
    static int GameRenderer_renderCurrentFrame(void* rend);
    static void Keyboard_feed(int key, bool isDown);
    static void onClick(ClickMap*, char clickType, char isDownWheelDelta, uintptr_t a4, int16_t a5, int16_t a6, int16_t, char a8);
    static BOOL __stdcall hkLoadLibraryW(LPCWSTR lib);
public:
    GenericHooks();
};