#pragma once
#include "util/LMath.h"

namespace sdk {
	class LevelRendererPlayer {
	public:
		CLASS_FIELD(Vec3, origin, 0x208);
		CLASS_FIELD(float, fovX, 0xD90);
		CLASS_FIELD(float, fovY, 0xDA4);
	};
}