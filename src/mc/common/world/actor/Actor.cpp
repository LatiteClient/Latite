#include "pch.h"
#include "Actor.h"
#include "mc/Addresses.h"
#include "mc/common/world/Attribute.h"
#include <mc/common/entity/component/AttributesComponent.h>

#include "mc/common/entity/component/ActorDataFlagComponent.h"
#include "mc/common/entity/component/ActorEquipmentComponent.h"
#include "mc/common/entity/component/ActorTypeComponent.h"
#include "mc/common/entity/component/RuntimeIDComponent.h"

SDK::ActorDataFlagComponent* SDK::Actor::getActorDataFlagsComponent() {
	return this->tryGetComponent<ActorDataFlagComponent>();
}

bool SDK::Actor::getStatusFlag(int flag) {
	auto comp = getActorDataFlagsComponent();
	if (comp == nullptr) {
		return false;
	}
	return comp->flags[flag];
}

void SDK::Actor::setStatusFlag(int flag, bool value) {
	auto comp = getActorDataFlagsComponent();
	if (comp == nullptr) {
		return;
	}
	comp->flags[flag] = value;
}

AABB& SDK::Actor::getBoundingBox() {
	return aabbShape->boundingBox;
}

Vec2& SDK::Actor::getRot() {
	return actorRotation->rotation;
}

Vec3& SDK::Actor::getVelocity() {
	return stateVector->velocity;
}

Vec3& SDK::Actor::getPos() {
	return stateVector->pos;
}

Vec3& SDK::Actor::getPosOld() {
	return stateVector->posOld;
}

int SDK::Actor::getCommandPermissionLevel() {
	// @dump-wbds vtable Actor, getCommandPermissionLevel
	return memory::callVirtual<int>(this, 0x66);
}

void SDK::Actor::setNameTag(std::string* nametag) {
	return reinterpret_cast<void(__fastcall*)(Actor*, std::string*)>(Signatures::Actor_setNameTag.result)(this, nametag);
}

uint64_t SDK::Actor::getRuntimeID() {
	return this->tryGetComponent<RuntimeIDComponent>()->runtimeID;
}

uint32_t SDK::Actor::getEntityTypeID() {
	return this->tryGetComponent<ActorTypeComponent>()->type;
}

void SDK::Actor::swing() {
	// @dump-wbds vtable Actor, swing
	return memory::callVirtual<void>(this, 0x6E);
}

bool SDK::Actor::isPlayer() {
	return getEntityTypeID() == 319;
}

bool SDK::Actor::isItem() {
	return getEntityTypeID() == 64;
}

SDK::AttributesComponent* SDK::Actor::getAttributesComponent() {
	return this->tryGetComponent<AttributesComponent>();
}

SDK::AttributeInstance* SDK::Actor::getAttribute(SDK::Attribute& attribute) {
	return getAttributesComponent()->baseAttributes.getInstance(attribute.mIDValue);
}

float SDK::Actor::getHealth() {
	auto attrib = getAttribute(SDK::Attributes::Health);
	if (!attrib)
		return 20.f;
	return attrib->value;
}

float SDK::Actor::getHunger() {
	auto attrib = getAttribute(SDK::Attributes::Hunger);
	if (!attrib)
		return 20.f;
	return attrib->value;
}

float SDK::Actor::getSaturation() {
	auto attrib = getAttribute(SDK::Attributes::Saturation);
	if (!attrib)
		return 20.f;
	return attrib->value;
}

bool SDK::Actor::isInvisible() {
	// @dump-wbds Actor, isInvisible
	return memory::callVirtual<bool>(this, 0x1F);
}

SDK::ItemStack* SDK::Actor::getArmor(int armorSlot) {
	return this->tryGetComponent<ActorEquipmentComponent>()->armorContainer->getItem(armorSlot);
}
