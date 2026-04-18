#pragma once
#include "mc/common/client/renderer/MaterialPtr.h"
#include "util/LMath.h"

namespace SDK {
	class LevelRendererPlayer {
	public:
		Vec3& getOrigin();
		float& getFovX();
		float& getFovY();
	};
}