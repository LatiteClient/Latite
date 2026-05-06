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
	return reinterpret_cast<SDK::ActorDataFlagComponent * (__fastcall*)(uintptr_t a1, uint32_t * a2)>(Signatures::Components::actorDataFlagsComponent.result)(entityContext.getBasicRegistry(), &entityContext.getId());
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

int64_t SDK::Actor::getRuntimeID() {
	return *reinterpret_cast<int64_t * (__fastcall*)(uintptr_t a1, uint32_t * a2)>(Signatures::Components::runtimeIDComponent.result)(entityContext.getBasicRegistry(), &entityContext.getId());
}

uint8_t SDK::Actor::getEntityTypeID() {
	return *reinterpret_cast<uint32_t * (__fastcall*)(uintptr_t a1, uint32_t * a2)>(Signatures::Components::actorTypeComponent.result)(entityContext.getBasicRegistry(), &entityContext.getId());
}

void SDK::Actor::swing() {
	// @dump-wbds vtable Actor, swing
	return memory::callVirtual<void>(this, 0x6E);
}

bool SDK::Actor::isPlayer() {
	return getEntityTypeID() == 63;
}

SDK::AttributesComponent* SDK::Actor::getAttributesComponent() {
	return reinterpret_cast<SDK::AttributesComponent * (__fastcall*)(const EntityContext&)>(Signatures::Components::attributesComponent.result)(entityContext);
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
	// TODO: this is EXTREMELY scuffed
	int& componentId = hat::member_at<int>(this, 0x18);
	auto obj = reinterpret_cast<void* (*)(void* obj, int& id)>(Signatures::Components::actorEquipmentPersistentComponent.result)
		(hat::member_at<void*>(this, 0x10), componentId);

	return (*(SDK::ItemStack*(**)(LPVOID, int))(**(uintptr_t**)((uintptr_t)obj + 8) + 56i64))(*(LPVOID*)((uintptr_t)obj + 8), armorSlot);
}
