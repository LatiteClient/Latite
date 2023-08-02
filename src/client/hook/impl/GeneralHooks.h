#pragma once
#include "client/hook/Hook.h"
#include "sdk/common/world/level/Level.h"
#include "util/Util.h"
#include "sdk/misc/ClickMap.h"

class GenericHooks : public HookGroup {
    static void __fastcall Level_tick(sdk::Level* level);
    static void* __fastcall ChatScreenController_sendChatMessage(void* controller, std::string& message);
    static void* __fastcall GameRenderer_renderCurrentFrame(void* rend);
    static void __fastcall Keyboard_feed(int key, bool isDown);
    static void __fastcall onClick(ClickMap*, char clickType, char isDownWheelDelta, uintptr_t a4, int16_t a5, int16_t a6, int16_t, char a8);
    static BOOL __stdcall hkLoadLibraryW(LPCWSTR lib);
public:
    GenericHooks();
};