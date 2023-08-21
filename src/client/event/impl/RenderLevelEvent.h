#pragma once
#include "api/eventing/Event.h"
#include "util/FNV32.h"
#include "sdk/common/client/renderer/LevelRenderer.h"

class RenderLevelEvent : public Event {
public:
	static const uint32_t hash = TOHASH(RenderLevelEvent);

	[[nodiscard]] SDK::LevelRenderer* getLevelRenderer() { return rend; }

	RenderLevelEvent(SDK::LevelRenderer* rend) : rend(rend) {}
private:
	SDK::LevelRenderer* rend;
};