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
		MVCLASS_FIELD(int32_t, ticksExisted, 0x228, 0x200, 0x200, 0x2A8, 0x24C);
		MVCLASS_FIELD(int32_t, invulnerableTime, 0x22C, 0x204, 0x204, 0x2AC, 0x250); // Mob::hurtEffects
		MVCLASS_FIELD(std::shared_ptr<class Dimension>, dimension, 0x278, 0x250, 0x250, 0x360, 0x300);
		MVCLASS_FIELD(StateVectorComponent*, stateVector, 0x2C8, 0x2A0, 0x2A0, 0, 0x350);
		MVCLASS_FIELD(AABBShapeComponent*, aabbShape, 0x2D0, 0x2A8, 0x2A8, 0, 0x358);
		MVCLASS_FIELD(MovementInterpolatorComponent*, movementInterpolator, 0x2D8, 0x2B0, 0x2B0, 0, 0x360);
		CLASS_FIELD(MolangVariableMap, molangVariableMap, 0x450);

		// 1.19.51 only
		CLASS_FIELD(class Level*, level_1_19_51, 0x310);

		// 1.20.40+ only
		MVCLASS_FIELD(class Inventory*, armorContainer, 0x450, 0x428, 0, 0, 0); // xref: Actor::getArmor(ArmorSlot)

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