#pragma once
#include "sdk/common/world/actor/player/Player.h"

namespace SDK {
	class LocalPlayer : public Player {
	public:
		void applyTurnDelta(const Vec2& delta);
	};
}