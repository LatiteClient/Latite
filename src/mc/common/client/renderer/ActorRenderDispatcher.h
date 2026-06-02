#pragma once

#include "mc/Util.h"
#include <memory>

namespace SDK {
	class ActorRenderer;

	class ActorRenderDispatcher {
	public:
		std::shared_ptr<ActorRenderer> getRendererById(void const* actorRendererId);
		void renderUI(class BaseActorRenderContext* baseActorRenderContext, class Actor* actor,
			Vec3 const& cameraTarget, Vec2 const& rotation, bool uiRendering);
	};
}
