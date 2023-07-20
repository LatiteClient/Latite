#pragma once
#include "util/LMath.h"

namespace sdk {
	class LevelRendererPlayer {
	public:
		Vec3& getOrigin();
		float& getFovX();
		float& getFovY();
	};
}