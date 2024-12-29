#pragma once
#include "sdk/Util.h"
#include "sdk/common/entity/EntityContext.h"
#include "sdk/common/util/MolangVariableMap.h"
#include <memory>

#include "sdk/common/entity/component/ActorDataFlagComponent.h"

namespace SDK {
	class Actor {
	public:
		Actor() = delete;

		CLASS_FIELD(EntityContext, entityContext, 0x8);
		MVCLASS_FIELD(int32_t, ticksExisted, 0x198, 0x1F0, 0x208, 0x200, 0x200, 0x2A8, 0x24C);
		MVCLASS_FIELD(int32_t, invulnerableTime, 0x19C, 0x1F4, 0x20C, 0x204, 0x204, 0x2AC, 0x250); // Mob::hurtEffects
		// @dump-wbds Actor::getDimensionConst, 9, -8
		MVCLASS_FIELD(std::shared_ptr<class Dimension>, dimension, 0x1C8, 0x220, 0x240, 0x250, 0x250, 0x360, 0x300);
		MVCLASS_FIELD(StateVectorComponent*, stateVector, 0x218, 0x270, 0x290, 0x2A0, 0x2A0, 0, 0x350);
		MVCLASS_FIELD(AABBShapeComponent*, aabbShape, 0x220, 0x278, 0x298, 0x2A8, 0x2A8, 0, 0x358);
		MVCLASS_FIELD(ActorRotationComponent*, actorRotation, 0x228, 0x280, 0x2A0, 0x2B0, 0x2B0, 0, 0x360);
		// @dump-wbds Actor::getMolangVariables, 3
		MVCLASS_FIELD(MolangVariableMap, molangVariableMap, 0x370, 0x3C8, 0x400, 0x450, 0, 0, 0);


		// 1.19.51 only
		CLASS_FIELD(class Level*, level_1_19_51, 0x310)
		// @dump-wbds Actor::getArmorContainer, 3
		MVCLASS_FIELD(class Inventory*, armorContainer, 0x0, 0x428, 0, 0, 0); // xref: Actor::getArmor(ArmorSlot)

		ActorDataFlagComponent* getActorDataFlagsComponent();

		bool getStatusFlag(int flag);
		void setStatusFlag(int flag, bool value);
		
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

		class AttributesComponent* getAttributesComponent();
		class AttributeInstance* getAttribute(class Attribute& attribute);
		float getHealth();
		float getHunger();
		float getSaturation();
		bool isInvisible();
		class ItemStack* getArmor(int armorSlot);
	};
}
