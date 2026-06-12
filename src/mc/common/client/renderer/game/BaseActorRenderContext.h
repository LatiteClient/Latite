#pragma once
#include "mc/Util.h"
#include <memory>

namespace SDK {
    class ActorRenderDispatcher;

    class BaseActorRenderContext {
        char pad[0x500]; // TODO: check actual size
    public:
        CLASS_FIELD(std::shared_ptr<ActorRenderDispatcher>, entityRenderDispatcher, 0x38);
        CLASS_FIELD(class ItemRenderer*, itemRenderer, 0x58);

        BaseActorRenderContext(class ScreenContext* ctx, class ClientInstance* cInst, class MinecraftGame* game);
        ~BaseActorRenderContext();
    };
}
