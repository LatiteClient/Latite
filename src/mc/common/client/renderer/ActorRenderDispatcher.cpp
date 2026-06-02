#include "pch.h"
#include "ActorRenderDispatcher.h"

#include "mc/Addresses.h"

std::shared_ptr<SDK::ActorRenderer> SDK::ActorRenderDispatcher::getRendererById(void const* actorRendererId) {
	std::shared_ptr<ActorRenderer> renderer{};
	if (!actorRendererId || !Signatures::ActorRenderDispatcher_getRendererById.result) return renderer;

	using oFunc_t = std::shared_ptr<ActorRenderer>*(__fastcall*)(ActorRenderDispatcher*, std::shared_ptr<ActorRenderer>*, void const*);
	auto oFunc = reinterpret_cast<oFunc_t>(Signatures::ActorRenderDispatcher_getRendererById.result);
	oFunc(this, &renderer, actorRendererId);
	return renderer;
}

void SDK::ActorRenderDispatcher::renderUI(SDK::BaseActorRenderContext* baseActorRenderContext, SDK::Actor* actor,
	Vec3 const& cameraTarget, Vec2 const& rotation, bool uiRendering) {
	using oFunc_t = void(__fastcall*)(ActorRenderDispatcher*, BaseActorRenderContext*, Actor*, Vec3 const&, Vec2 const&, bool);
	static auto oFunc = reinterpret_cast<oFunc_t>(Signatures::ActorRenderDispatcher_renderUI.result);
	oFunc(this, baseActorRenderContext, actor, cameraTarget, rotation, uiRendering);
}
