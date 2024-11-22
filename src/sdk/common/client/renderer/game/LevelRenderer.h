#pragma once
#include "sdk/Util.h"
#include "LevelRendererPlayer.h"

namespace SDK {
	class LevelRenderer {
		MVCLASS_FIELD(LevelRendererPlayer*, levelRendererPlayer, 0x318, 0x308, 0x308, 0x308, 0x308, 0x308, 0x288, 0x2A8);
	public:
		LevelRendererPlayer* getLevelRendererPlayer() {
			if (SDK::internalVers <= V1_19_51 /*1.20+*/) {
				return reinterpret_cast<LevelRendererPlayer*>(&levelRendererPlayer);
			}
			return levelRendererPlayer;
		}
	};
}