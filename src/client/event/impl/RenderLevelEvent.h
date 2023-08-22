/* Copyright (C) Imrglop, All rights reserved
 * Copyright (C) Latite Client contributors, All rights reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited without the permission of the author.
 * Proprietary and confidential
 * Written by Imrglop <contact: latiteclientgithub@gmail.com>, 2023
 */

#pragma once
#include "api/eventing/Event.h"
#include "util/FNV32.h"
#include "sdk/common/client/renderer/game/LevelRenderer.h"

class RenderLevelEvent : public Event {
public:
	static const uint32_t hash = TOHASH(RenderLevelEvent);

	[[nodiscard]] SDK::LevelRenderer* getLevelRenderer() { return rend; }

	RenderLevelEvent(SDK::LevelRenderer* rend) : rend(rend) {}
private:
	SDK::LevelRenderer* rend;
};