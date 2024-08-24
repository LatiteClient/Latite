#include "pch.h"
#include "Actor.h"
#include "sdk/signature/storage.h"
#include "sdk/common/world/Attribute.h"
#include <sdk/common/entity/component/AttributesComponent.h>

#include "ActorCollision.h"

SDK::ActorDataFlagComponent* SDK::Actor::getActorDataFlagsComponent() {
	return reinterpret_cast<SDK::ActorDataFlagComponent * (__fastcall*)(uintptr_t a1, uint32_t * a2)>(Signatures::Components::actorDataFlagsComponent.result)(entityContext.getBasicRegistry(), &entityContext.getId());
}

bool SDK::Actor::getStatusFlag(int flag) {
	if (internalVers < V1_21_20) {
		return memory::callVirtual<bool>(this, 0, flag);
	}
	auto comp = getActorDataFlagsComponent();
	if (comp == nullptr) {
		return false;
	}
	return comp->flags[flag];
}

void SDK::Actor::setStatusFlag(int flag, bool value) {
	if (internalVers < V1_21_20) {
		return memory::callVirtual<void>(this, 1, flag, value);
	}
	auto comp = getActorDataFlagsComponent();
	if (comp == nullptr) {
		return;
	}
	comp->flags[flag] = value;
}

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
	return actorRotation->rotation;
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
	// @dump-wbds vtable Actor, getCommandPermissionLevel
	return memory::callVirtual<int>(this, mvGetOffset<0x6B, 0x6D, 0x7C, 0xB5, 0xCC, 0xCD>());
}

int64_t SDK::Actor::getRuntimeID() {
	if (internalVers < V1_19_51) {
		return util::directAccess<int64_t>(this, 0x550);
	}
	return *reinterpret_cast<int64_t * (__fastcall*)(uintptr_t a1, uint32_t * a2)>(Signatures::Components::runtimeIDComponent.result)(entityContext.getBasicRegistry(), &entityContext.getId());
}

uint8_t SDK::Actor::getEntityTypeID() {
	if (internalVers >= V1_20_50) {
		return static_cast<uint32_t>(*reinterpret_cast<uint32_t * (__fastcall*)(uintptr_t a1, uint32_t * a2)>(Signatures::Components::actorTypeComponent.result)(entityContext.getBasicRegistry(), &entityContext.getId()));
	}

	return memory::callVirtual<int>(this, mvGetOffset<0x64, 0x97, 0xAA, 0xAC>());
}

void SDK::Actor::swing() {
	// @dump-wbds vtable Actor, swing
	return memory::callVirtual<void>(this, SDK::mvGetOffset<0x73, 0x75, 0x86, 0xC4, 0xDB, 0xDC>());
}

bool SDK::Actor::isPlayer() {
	return getEntityTypeID() == 63;
}

SDK::AttributesComponent* SDK::Actor::getAttributesComponent() {
	return reinterpret_cast<SDK::AttributesComponent * (__fastcall*)(uintptr_t a1, uint32_t * a2)>(Signatures::Components::attributesComponent.result)(entityContext.getBasicRegistry(), &entityContext.getId());
}

SDK::AttributeInstance* SDK::Actor::getAttribute(SDK::Attribute& attribute) {
	if (internalVers >= V1_20_40) {
		return getAttributesComponent()->baseAttributes.getInstance(attribute.mIDValue);
	}

	return memory::callVirtual<SDK::AttributeInstance*>(this, SDK::mvGetOffset<0xB8, 0xCF, 0xD0>(), attribute);
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
	return memory::callVirtual<bool>(this, SDK::mvGetOffset<0x20, 0x22, 0x25, 0x34, 0x3B, 0x3D>());
}

SDK::ItemStack* SDK::Actor::getArmor(int armorSlot) {
	if (internalVers >= V1_20_80) {
		// TODO: this is EXTREMELY scuffed
		int& componentId = util::directAccess<int>(this, 0x18);
		auto obj = reinterpret_cast<void* (*)(void* obj, int& id)>(Signatures::Components::actorEquipmentPersistentComponent.result)
			(util::directAccess<void*>(this, 0x10), componentId);

		return (*(SDK::ItemStack*(**)(LPVOID, int))(**(uintptr_t**)((uintptr_t)obj + 8) + 56i64))(*(LPVOID*)((uintptr_t)obj + 8), armorSlot);
	}

	if (internalVers >= V1_20_40) {
		return this->armorContainer->getItem(armorSlot);
	}

	return memory::callVirtual<ItemStack*>(this, mvGetOffset<0x71, 0xB5, 0xCC, 0xCD>());
}

bool SDK::Actor::isOnGround() {
	return ActorCollision::isOnGround(entityContext);
}
