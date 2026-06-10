#include "pch.h"
#include "Actor.h"
#include "mc/Addresses.h"
#include "mc/common/world/Attribute.h"
#include <mc/common/entity/component/AttributesComponent.h>

#include "mc/common/entity/component/ActorDataFlagComponent.h"
#include "mc/common/entity/component/ActorEquipmentComponent.h"
#include "mc/common/entity/component/ActorTypeComponent.h"
#include "mc/common/entity/component/MobBodyRotationComponent.h"
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

void SDK::Actor::setUIRendering(bool value) {
	return reinterpret_cast<void(__fastcall*)(Actor*, bool)>(Signatures::Actor_setUIRendering.result)(this, value);
}

void SDK::Actor::setYHeadRotations(float current, float old) {
	return reinterpret_cast<void(__fastcall*)(Actor*, float, float)>(Signatures::Actor_setYHeadRotations.result)(this, current, old);
}

void SDK::Actor::setYBodyRotations(float current, float old) {
	return reinterpret_cast<void(__fastcall*)(Actor*, float, float)>(Signatures::Mob_setYBodyRotations.result)(this, current, old);
}

void SDK::Actor::setRotationY(float value) {
	return reinterpret_cast<void(__fastcall*)(Actor*, float)>(Signatures::Actor_setRotationY.result)(this, value);
}

uint64_t SDK::Actor::getRuntimeID() {
	return this->tryGetComponent<RuntimeIDComponent>()->runtimeID;
}

uint32_t SDK::Actor::getEntityTypeID() {
	return this->tryGetComponent<ActorTypeComponent>()->type;
}

void const* SDK::Actor::getActorRendererId() {
	// render gets the usual renderer id from vfunc 69 right now
	return memory::callVirtual<void const*>(this, 0x45);
}

void const* SDK::Actor::getActorRendererIdOverride() {
	// some actors use this one instead of the query renderer id
	return memory::callVirtual<void const*>(this, 0x52);
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

std::optional<float> SDK::Actor::getHealth() {
	auto attrib = getAttribute(SDK::Attributes::Health);
	if (!attrib)
		return std::nullopt;
	return attrib->value;
}

std::optional<float> SDK::Actor::getMaxHealth() {
	auto attrib = getAttribute(SDK::Attributes::Health);
	if (!attrib)
		return std::nullopt;
	return attrib->maxValue;
}

std::optional<float> SDK::Actor::getHunger() {
	auto attrib = getAttribute(SDK::Attributes::Hunger);
	if (!attrib)
		return std::nullopt;
	return attrib->value;
}

std::optional<float> SDK::Actor::getSaturation() {
	auto attrib = getAttribute(SDK::Attributes::Saturation);
	if (!attrib)
		return std::nullopt;
	return attrib->value;
}

bool SDK::Actor::isInvisible() {
	// @dump-wbds Actor, isInvisible
	return memory::callVirtual<bool>(this, 0x1F);
}

SDK::ItemStack* SDK::Actor::getArmor(int armorSlot) {
	return this->tryGetComponent<ActorEquipmentComponent>()->armorContainer->getItem(armorSlot);
}
