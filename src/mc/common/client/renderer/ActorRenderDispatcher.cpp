#include "pch.h"
#include "ActorRenderDispatcher.h"

#include "mc/Addresses.h"

void SDK::ActorRenderDispatcher::renderUI(SDK::BaseActorRenderContext* baseActorRenderContext, SDK::Actor* actor,
                                          Vec3 const& cameraTarget, Vec2 const& rotation, bool uiRendering) {
    using oFunc_t = void(__fastcall*)(ActorRenderDispatcher*, BaseActorRenderContext*, Actor*, Vec3 const&, Vec3 const&,
                                      Vec2 const&, bool);
    static auto oFunc = reinterpret_cast<oFunc_t>(Signatures::ActorRenderDispatcher_render.result);
    oFunc(this, baseActorRenderContext, actor, {}, cameraTarget, rotation, uiRendering);
}
