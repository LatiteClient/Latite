#pragma once
#include "sdk/Util.h"
#include "sdk/common/entity/EntityContext.h"
#include "sdk/common/util/MolangVariableMap.h"
#include <memory>

namespace SDK {
	class Actor {
	public:
		Actor() = delete;

		CLASS_FIELD(EntityContext, entityContext, 0x8);
		MVCLASS_FIELD(int32_t, ticksExisted, 0x200, 0x2A8, 0x24C);
		MVCLASS_FIELD(int32_t, invulnerableTime, 0x204, 0x2AC, 0x250);
		MVCLASS_FIELD(std::shared_ptr<class Dimension>, dimension, 0x250, 0x360, 0x300);
		MVCLASS_FIELD(StateVectorComponent*, stateVector, 0x2A0, 0, 0x350);
		MVCLASS_FIELD(AABBShapeComponent*, aabbShape, 0x2A8, 0, 0x358);
		MVCLASS_FIELD(MovementInterpolatorComponent*, movementInterpolator, 0x2B0, 0, 0x360);
		CLASS_FIELD(MolangVariableMap, molangVariableMap, 0x450);

		// 1.19.51 only
		CLASS_FIELD(class Level*, level_1_19_51, 0x310);

		// 1.20.40+ only
		CLASS_FIELD(class Inventory*, armorContainer, 0x428); // xref: Actor::getArmor(ArmorSlot)

		AABB& getBoundingBox();
		Vec2& getRot();
		Vec3& getVelocity();
		Vec3& getPos();
		Vec3& getPosOld();

		int getCommandPermissionLevel();
		int64_t getRuntimeID();
		uint8_t getEntityTypeID();
		void swing();
		bool isPlayer();
		class AttributeInstance* getAttribute(class Attribute& attribute);
		float getHealth();
		float getHunger();
		float getSaturation();
		class ItemStack* getArmor(int armorSlot);
	};
}