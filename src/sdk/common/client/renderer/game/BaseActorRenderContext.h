#pragma once
#include "sdk/Util.h"

namespace SDK {
    class BaseActorRenderContext {
        char pad[0x500]; // TODO: check actual size
    public:
        MVCLASS_FIELD(class ItemRenderer*, itemRenderer, 0x58, 0x58, 0x60);

        BaseActorRenderContext(class ScreenContext* ctx, class ClientInstance* cInst, class MinecraftGame* game);
        ~BaseActorRenderContext();
    };
}