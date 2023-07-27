#pragma once
#include "sdk/util.h"
#include "sdk/common/entity/EntityContext.h"
#include <memory>

namespace sdk {
	class Actor {
	public:
		Actor() = delete;

		CLASS_FIELD(EntityContext, entityContext, 0x8);
		MVCLASS_FIELD(int32_t, ticksExisted, 0x200, 0x2A8, 0x24C);
		MVCLASS_FIELD(int32_t, invulnerableTime, 0x204, 0x2AC, 0x250);
		MVCLASS_FIELD(std::shared_ptr<class Dimension>, dimension, 0x250, 0x360, 0x300);
		MVCLASS_FIELD(StateVectorComponent, stateVector, 0x2A0, 0, 0x350);
		MVCLASS_FIELD(AABBShapeComponent*, aabbShape, 0x2A8, 0, 0x358);
		MVCLASS_FIELD(MovementInterpolatorComponent*, movementInterpolator, 0x2B0, 0, 0x360);

		// 1.19.51 only
		CLASS_FIELD(class Level*, level_1_19_51, 0x310);

		AABB& getBoundingBox() {
			if (internalVers < V1_19_51) {
				return util::directAccess<AABB>(this, 0x4B8);
			}
			return aabbShape->boundingBox;
		}

		Vec2& getRot() { 
			if (internalVers < V1_19_51) {
				return util::directAccess<Vec2>(this, 0x138);
			}
			return movementInterpolator->rotation;
		}

		Vec3& getVelocity() {
			if (internalVers < V1_19_51) {
				return util::directAccess<Vec3>(this, 0x4F0);
			}
			return stateVector.velocity;
		}
	};
}