#pragma once
#include "mc/Util.h"
#include "mc/common/entity/EntityContext.h"
#include "mc/common/util/MolangVariableMap.h"
#include <memory>

#include "mc/common/entity/component/ActorDataFlagComponent.h"

namespace SDK {
	class Actor {
	public:
		Actor() = delete;

		CLASS_FIELD(EntityContext, entityContext, 0x8);
		CLASS_FIELD(int32_t, ticksExisted, 0x198);
		CLASS_FIELD(int32_t, invulnerableTime, 0x19C); // Mob::hurtEffects
		// @dump-wbds Actor::getDimensionConst, 9, -8
		CLASS_FIELD(std::shared_ptr<class Dimension>, dimension, 0x1C8);
		CLASS_FIELD(StateVectorComponent*, stateVector, 0x218);
		CLASS_FIELD(AABBShapeComponent*, aabbShape, 0x220);
		CLASS_FIELD(ActorRotationComponent*, actorRotation, 0x228);
		// @dump-wbds Actor::getMolangVariables, 3
		CLASS_FIELD(MolangVariableMap, molangVariableMap, 0x370);

		ActorDataFlagComponent* getActorDataFlagsComponent();

		bool getStatusFlag(int flag);
		void setStatusFlag(int flag, bool value);
		
		AABB& getBoundingBox();
		Vec2& getRot();
		Vec3& getVelocity();
		Vec3& getPos();
		Vec3& getPosOld();

		int getCommandPermissionLevel();
		void setNameTag(std::string* nametag);
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
