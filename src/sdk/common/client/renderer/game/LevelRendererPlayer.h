#pragma once
#include "util/LMath.h"

namespace SDK {
	class LevelRendererPlayer {
	public:
		Vec3& getOrigin();
		float& getFovX();
		float& getFovY();
	};
}