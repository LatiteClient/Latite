#pragma once
#include <cstdint>
#include <bitset>
#include "util/LMath.h"

namespace SDK {
	struct EntityContext_Old {
		struct {
			uintptr_t basicRegistry;
		}*basicRegistry;
		uint32_t id;
	};

	class EntityContext {
	public:
		uintptr_t getBasicRegistry() {
			if (internalVers >= V1_20_50) {
				return basicRegistry;
			}

			return reinterpret_cast<EntityContext_Old*>(this)->basicRegistry->basicRegistry;
		}

		uint32_t& getId() {
			if (internalVers >= V1_20_50) {
				return id;
			}

			return reinterpret_cast<EntityContext_Old*>(this)->id;

		}
	private:
		void* entityRegistry;
		uintptr_t basicRegistry;
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

	class ActorRotationComponent
	{
	public:
		Vec2 rotation; //0x0000
		Vec2 rotationOld; //0x0008
	}; //Size: 0x0010

	struct ActorDataFlagComponent {
		std::bitset<119> flags;
	};
}
