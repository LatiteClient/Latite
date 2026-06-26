#pragma once

#include "mc/Util.h"
#include <memory>

namespace SDK {
    class ActorRenderer;

    class ActorRenderDispatcher {
    public:
        void renderUI(class BaseActorRenderContext* baseActorRenderContext, class Actor* actor,
                      Vec3 const& cameraTarget, Vec2 const& rotation, bool uiRendering);
    };
}
