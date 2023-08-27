#pragma once
#include "util/LMath.h"

namespace SDK {
	enum struct HitType : int {
		BLOCK = 0,
		ENTITY = 1,
		AIR = 3
	};

	class HitResult {
	public:
		Vec3 start;
		Vec3 end;
		HitType hitType;
		int8_t face;
		BlockPos hitBlock;
		Vec3 hitPos;
		// ...
	};
}