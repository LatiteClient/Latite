#pragma once
#include "sdk/util.h"
#include "sdk/common/entity/EntityContext.h"

namespace sdk {
	class Actor {
	public:
		Actor() = delete;

		CLASS_FIELD(EntityContext, entityContext, 0x8);
		CLASS_FIELD(int32_t, ticksExisted, 0x200);
		CLASS_FIELD(int32_t, invulnerableTime, 0x204);
		CLASS_FIELD(std::shared_ptr<class Dimension>, dimension, 0x250);
		CLASS_FIELD(StateVectorComponent, stateVector, 0x2A0);
		CLASS_FIELD(AABBShapeComponent*, aabbShape, 0x2A8);
		CLASS_FIELD(MovementInterpolatorComponent*, movementInterpolator, 0x2B0);

		AABB& getBoundingBox() { return aabbShape->boundingBox; }
		Vec2& getRot() { return movementInterpolator->rotation; };
		Vec3 getVelocity() { return stateVector.velocity; };
	};
}