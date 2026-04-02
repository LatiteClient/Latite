#pragma once
#include "mc/Util.h"
#include "LevelRendererPlayer.h"

namespace SDK {
	class LevelRenderer {
		CLASS_FIELD(LevelRendererPlayer*, levelRendererPlayer, 0x448);
	public:
		LevelRendererPlayer* getLevelRendererPlayer() {
			if (SDK::internalVers <= V1_19_51 /*1.20+*/) {
				return reinterpret_cast<LevelRendererPlayer*>(&levelRendererPlayer);
			}
			return levelRendererPlayer;
		}
	};
}