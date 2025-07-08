#pragma once
#include "client/event/Event.h"
#include "util/Crypto.h"
#include "mc/common/client/renderer/game/LevelRenderer.h"

class RenderLevelEvent : public Event {
public:
	static const uint32_t hash = TOHASH(RenderLevelEvent);

	[[nodiscard]] SDK::LevelRenderer* getLevelRenderer() { return rend; }
	[[nodiscard]] SDK::ScreenContext* getScreenContext() { return screen; }

	RenderLevelEvent(SDK::LevelRenderer* rend, SDK::ScreenContext* ctx) : rend(rend), screen(ctx) {}
private:
	SDK::LevelRenderer* rend;
	SDK::ScreenContext* screen;
};