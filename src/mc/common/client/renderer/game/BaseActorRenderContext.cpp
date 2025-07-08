#include "pch.h"
#include "BaseActorRenderContext.h"
#include "mc/Addresses.h"

SDK::BaseActorRenderContext::BaseActorRenderContext(SDK::ScreenContext* ctx, SDK::ClientInstance* cInst, SDK::MinecraftGame* game) {
    using oFunc_t = BaseActorRenderContext * (*)(BaseActorRenderContext*, ScreenContext*, ClientInstance*, MinecraftGame*);
	memset(this, 0, sizeof(SDK::BaseActorRenderContext));
	static auto oFunc = reinterpret_cast<oFunc_t>(Signatures::BaseActorRenderContext_BaseActorRenderContext.result);
	oFunc(this, ctx, cInst, game);
}

SDK::BaseActorRenderContext::~BaseActorRenderContext() {
}
