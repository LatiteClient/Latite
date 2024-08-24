#include "pch.h"
#include "ActorCollision.h"

bool SDK::ActorCollision::isOnGround(EntityContext& ctx) {
    using isOnGroundFunc = bool(__fastcall *)(uintptr_t a1, uint32_t * a2);
    static auto isOnGround = reinterpret_cast<isOnGroundFunc>(Signatures::ActorCollision_isOnGround.result);
    
    if (isOnGround)
        return isOnGround(ctx.getBasicRegistry(), &ctx.getId());

    return false;
}
