#pragma once
#include "sdk/util.h"
#include "LevelRendererPlayer.h"

namespace sdk {
	class LevelRenderer {
	public:
		MVCLASS_FIELD(LevelRendererPlayer*, levelRendererPlayer, 0x2F8, 0x288);
	};
}