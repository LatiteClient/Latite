#pragma once
#include "api/eventing/Event.h"
#include "util/chash.h"
#include "sdk/common/client/renderer/LevelRenderer.h"

class RenderLevelEvent : public Event {
public:
	static const uint32_t hash = TOHASH(RenderLevelEvent);

	[[nodiscard]] sdk::LevelRenderer* getLevelRenderer() { return rend; }

	RenderLevelEvent(sdk::LevelRenderer* rend) : rend(rend) {}
private:
	sdk::LevelRenderer* rend;
};