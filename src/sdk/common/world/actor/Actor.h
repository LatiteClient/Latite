#pragma once
#include "sdk/util.h"
#include "sdk/common/entity/EntityContext.h"
#include <memory>

namespace sdk {
	class Actor {
	public:
		Actor() = delete;

		CLASS_FIELD(EntityContext, entityContext, 0x8);
		MVCLASS_FIELD(int32_t, ticksExisted, 0x200, 0x2A8);
		MVCLASS_FIELD(int32_t, invulnerableTime, 0x204, 0x2AC);
		MVCLASS_FIELD(std::shared_ptr<class Dimension>, dimension, 0x250, 0x360);
		CLASS_FIELD(StateVectorComponent, stateVector, 0x2A0);
		CLASS_FIELD(AABBShapeComponent*, aabbShape, 0x2A8);
		CLASS_FIELD(MovementInterpolatorComponent*, movementInterpolator, 0x2B0);

		AABB& getBoundingBox() {
			if (internalVers == V1_18_12) {
				return util::directAccess<AABB>(this, 0x4B8);
			}
			return aabbShape->boundingBox;
		}

		Vec2& getRot() { 
			if (internalVers == V1_18_12) {
				return util::directAccess<Vec2>(this, 0x138);
			}
			return movementInterpolator->rotation;
		}

		Vec3& getVelocity() {
			if (internalVers == V1_18_12) {
				return util::directAccess<Vec3>(this, 0x4F0);
			}
			return stateVector.velocity;
		}
	};
}