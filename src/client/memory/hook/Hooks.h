#pragma once
#include "Hook.h"
#include "client/manager/StaticManager.h"

#include "hooks/GeneralHooks.h"
#include "hooks/LevelRendererHooks.h"
#include "hooks/OptionHooks.h"
#include "hooks/DXHooks.h"
#include "hooks/MinecraftGameHooks.h"
#include "hooks/RenderControllerHooks.h"
#include "hooks/ScreenViewHooks.h"
#include "hooks/PacketHooks.h"
#include "hooks/PlayerHooks.h"
#include "hooks/CustomSkinPickerHooks.h"

class LatiteHooks final : public StaticManager<HookGroup, GenericHooks, LevelRendererHooks, OptionHooks, DXHooks,
                                               PlayerHooks, MinecraftGameHooks, RenderControllerHooks, ScreenViewHooks,
                                               PacketHooks, CustomSkinPickerHooks> {
public:
    LatiteHooks();
    ~LatiteHooks();

    void enable();
    void disable();

private:
};
