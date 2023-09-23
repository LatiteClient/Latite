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
	if (internalVers <= V1_18_12) {
		return memory::callVirtual<int>(this, 0xCC);
	}

	if (internalVers <= V1_19_51) {
		return memory::callVirtual<int>(this, 0xCD);
	}

	return memory::callVirtual<int>(this, 0xB5);
}

int64_t SDK::Actor::getRuntimeID() {
	if (internalVers < V1_19_51) {
		return util::directAccess<int64_t>(this, 0x550);
	}
	return *reinterpret_cast<int64_t * (__fastcall*)(uintptr_t a1, uint32_t * a2)>(Signatures::Components::runtimeIDComponent.result)(entityContext.registry->basicRegistry, &entityContext.id);
}

uint8_t SDK::Actor::getEntityTypeID() {
	if (internalVers <= V1_18_12) {
		return memory::callVirtual<int>(this, 0xAA);
	}
	if (internalVers <= V1_19_51) {
		return memory::callVirtual<int>(this, 0xAC);
	}

	return memory::callVirtual<int>(this, 0x97);
}

void SDK::Actor::swing() {
	return memory::callVirtual<void>(this, MV_DETAIL_GETOFFSET(0xC4, 0xDB, 0xDC));
}

bool SDK::Actor::isPlayer() {
	return getEntityTypeID() == 63;
}

SDK::AttributeInstance* SDK::Actor::getAttribute(SDK::Attribute& attribute) {
	return memory::callVirtual<SDK::AttributeInstance*>(this, MV_DETAIL_GETOFFSET(0xB8, 0xCF, 0xD0), attribute);
}

float SDK::Actor::getHealth() {
	return getAttribute(SDK::Attributes::Health)->value;
}

float SDK::Actor::getHunger() {
	return getAttribute(SDK::Attributes::Hunger)->value;
}

float SDK::Actor::getSaturation() {
	return getAttribute(SDK::Attributes::Saturation)->value;
}
