#pragma once
#include "client/hook/Hook.h"
#include "sdk/common/world/level/Level.h"
#include "util/Util.h"
#include "sdk/misc/ClickMap.h"

class GenericHooks : public HookGroup {
    static void __fastcall Level_tick(SDK::Level* level);
    static void* __fastcall ChatScreenController_sendChatMessage(void* controller, std::string& message);
    static void* __fastcall GameRenderer_renderCurrentFrame(void* rend);
    static void __fastcall Keyboard_feed(int key, bool isDown);
    static void __fastcall onClick(ClickMap*, char clickType, char isDownWheelDelta, uintptr_t a4, int16_t a5, int16_t a6, int16_t, char a8);
    static BOOL __stdcall hkLoadLibraryW(LPCWSTR lib);
    static void* __fastcall RakNetConnector_tick(void* connector);
    static int __fastcall RakPeer_getAveragePing(void* obj, char* guidOrAddy);
    static void __fastcall LocalPlayer_applyTurnDelta(void* obj, Vec2& vec);
    static void __fastcall MoveInputHandler_tick(void* obj, void* proxy);
    static void __fastcall MovePlayer(
        uintptr_t** a1,
        void* a2,
        uintptr_t* a3,
        uintptr_t a4,
        uintptr_t a5,
        uintptr_t a6,
        uintptr_t a7,
        uintptr_t a8,
        uintptr_t a9,
        uintptr_t a10,
        uintptr_t a11);
    static void __fastcall CameraViewBob(void*, void*, void*);
public:
    GenericHooks();
};