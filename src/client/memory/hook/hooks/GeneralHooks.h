#pragma once
#include "mc/common/world/level/Level.h"
#include <mc/common/client/renderer/game/BaseActorRenderContext.h>
#include <mc/common/world/Weather.h>
#include <mc/common/client/renderer/game/CameraComponent.h>

class GenericHooks : public HookGroup {
    static void __fastcall Level_tick(SDK::Level* level);
    static void* __fastcall ChatScreenController_sendChatMessage(void* controller, std::string& message);
    static void* __fastcall GameRenderer_renderCurrentFrame(void* rend);
    static LRESULT __fastcall MainWindow__windowProcCallback(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    static bool __fastcall GameCore_handleMouseInput(void* a1, void* a2, void* a3);
    static BOOL __stdcall hkLoadLibraryW(LPCWSTR lib);
    static int __fastcall RakPeer_getAveragePing(void* obj, char* guidOrAddy);
    static void __fastcall LocalPlayer_applyTurnDelta(void* obj, Vec2& vec);
    static void __fastcall MoveInputHandler_tick(void* obj, void* proxy);
    static void __fastcall ClientInputUpdateSystem_tickBaseInput(
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
        uintptr_t a11,
        char a12,
        char a13,
        char a14);
    static void __fastcall CameraViewBob(void*, void*, void*);
    static bool Level_initialize(SDK::Level* obj, void* palette, void* settings, void* tickRange, void* experiments, uint64_t a6);
    static void* Level_startLeaveGame(SDK::Level* obj);
    static void* ActorRenderDispatcher_render(void* obj, SDK::BaseActorRenderContext* barc, SDK::Actor* entity, Vec3& pos3, Vec3 const& pos2, void* unk, bool affectedByLighting);
    static void LevelRendererPlayer_renderOutlineSelection(SDK::LevelRendererPlayer* obj, SDK::ScreenContext* scn, void* block, void* region, BlockPos pos);
    static void* hkRenderGuiItemNew(void* obj, SDK::BaseActorRenderContext* baseActorRenderContext, SDK::ItemStack* itemStack, int mode, float x, float y, float opacity, float scale, float a9, bool ench);
    static float hkGetTimeOfDay(SDK::Dimension* obj, int time, float a);
    static void hkDimensionTick(SDK::Dimension* obj);
    static Color* hkGetFogColor(SDK::Dimension* obj, Color* out, SDK::Actor* ent, float f);
    static void hkAddMessage(SDK::GuiData* obj, void* msg, uint32_t profanityContext);
    static void hkUpdatePlayer(SDK::CameraComponent* obj, void* a, void* b);
    static void hkOnUri(void* obj, void* uri);
    static void hkBobHurt(void* obj, void*, void*);
    static void hkGrabCursor(SDK::ClientInstance* obj);
public:
    GenericHooks();
};