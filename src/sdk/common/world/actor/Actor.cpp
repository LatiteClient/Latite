#include "pch.h"
#include "Actor.h"
#include "sdk/signature/storage.h"
#include "sdk/common/world/Attribute.h"

AABB& SDK::Actor::getBoundingBox() {
	if (internalVers < V1_19_51) {
		return util::directAccess<AABB>(this, 0x4B8);
	}
	return aabbShape->boundingBox;
}

Vec2& SDK::Actor::getRot() {
	if (internalVers < V1_19_51) {
		return util::directAccess<Vec2>(this, 0x138);
	}
	return movementInterpolator->rotation;
}

Vec3& SDK::Actor::getVelocity() {
	if (internalVers < V1_19_51) {
		return util::directAccess<Vec3>(this, 0x4F0);
	}
	return stateVector->velocity;
}

Vec3& SDK::Actor::getPos() {
	if (internalVers < V1_19_51) {
		return memory::callVirtual<Vec3&>(this, 22);
	}

	return stateVector->pos;
}

Vec3& SDK::Actor::getPosOld() {
	if (internalVers < V1_19_51) {
		return memory::callVirtual<Vec3&>(this, 23);
	}

	return stateVector->posOld;
}

int SDK::Actor::getCommandPermissionLevel() {
	return memory::callVirtual<int>(this, mvGetOffset<0x7A, 0x7C, 0xB5, 0xCC, 0xCD>());
}

int64_t SDK::Actor::getRuntimeID() {
	if (internalVers < V1_19_51) {
		return util::directAccess<int64_t>(this, 0x550);
	}
	return *reinterpret_cast<int64_t * (__fastcall*)(uintptr_t a1, uint32_t * a2)>(Signatures::Components::runtimeIDComponent.result)(entityContext.getBasicRegistry(), &entityContext.getId());
}

uint8_t SDK::Actor::getEntityTypeID() {
	if (internalVers >= V1_20_50) {
		return static_cast<uint8_t>(*reinterpret_cast<uint32_t * (__fastcall*)(uintptr_t a1, uint32_t * a2)>(Signatures::Components::runtimeIDComponent.result)(entityContext.getBasicRegistry(), &entityContext.getId()));
	}

	return memory::callVirtual<int>(this, mvGetOffset<0x64, 0x97, 0xAA, 0xAC>());
}

void SDK::Actor::swing() {
	return memory::callVirtual<void>(this, SDK::mvGetOffset<0x84, 0x86, 0xC4, 0xDB, 0xDC>());
}

bool SDK::Actor::isPlayer() {
	return getEntityTypeID() == 63;
}

SDK::AttributeInstance* SDK::Actor::getAttribute(SDK::Attribute& attribute) {
	if (internalVers >= V1_20_40) {
		return reinterpret_cast<AttributeInstance*(__fastcall*)(Actor*, Attribute&)>(Signatures::Actor_getAttribute.result)(this, attribute);
	}

	return memory::callVirtual<SDK::AttributeInstance*>(this, SDK::mvGetOffset<0xB8, 0xCF, 0xD0>(), attribute);
}

float SDK::Actor::getHealth() {
	return 20.f;
	return getAttribute(SDK::Attributes::Health)->value;
}

float SDK::Actor::getHunger() {
	return 20.f;
	return getAttribute(SDK::Attributes::Hunger)->value;
}

float SDK::Actor::getSaturation() {
	return 20.f;
	return getAttribute(SDK::Attributes::Saturation)->value;
}

SDK::ItemStack* SDK::Actor::getArmor(int armorSlot) {
	if (internalVers >= V1_20_40) {
		return this->armorContainer->getItem(armorSlot);
	}

	return memory::callVirtual<ItemStack*>(this, mvGetOffset<0x7C, 0xB5, 0xCC, 0xCD>());
}
