#pragma once
#include <cstdint>
#include "util/LMath.h"

namespace SDK {
	class EntityContext {
	public:
		struct {
			uintptr_t basicRegistry;
		}*registry;

		uint32_t id;
	};

	struct AABBShapeComponent
	{
	public:
		AABB boundingBox;
		Vec2 size;
	}; //Size: 0x0020

	class StateVectorComponent
	{
	public:
		Vec3 pos; //0x0000
		Vec3 posOld; //0x000C
		Vec3 velocity; //0x0018
	}; //Size: 0x0024

	class MovementInterpolatorComponent
	{
	public:
		Vec2 rotation; //0x0000
		Vec2 rotationOld; //0x0008
	}; //Size: 0x0010
}
