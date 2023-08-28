#include "pch.h"
#include "Level.h"
#include "sdk/version.h"
#include "api/memory/memory.h"

void SDK::Level::playSoundEvent(std::string const& text, Vec3 const& pos, float vol, float pitch) {
	int index = 0;
	switch (SDK::internalVers) {
	case V1_18_12:
		index = 0xBD;
		break;
	case V1_19_41:
	case V1_19_51:
		index = 0xC9;
		break;
	default:
		index = 0xA4;
		break;
	}

	memory::callVirtual<void>(this, index, text, pos, vol, pitch);
}

std::vector<SDK::Actor*> SDK::Level::getRuntimeActorList() {
	std::vector<Actor*> list;

	int index = 0;
	switch (SDK::internalVers) {
	case V1_18_12:
		index = 0x12D;
		break;
	//case V1_19_41:
	case V1_19_51:
		index = 0x13C;
		break;
	default:
		index = 0x129;
		break;
	}

	memory::callVirtual<void, std::vector<Actor*>&>(this, index, list);
	return list;
}

SDK::HitResult* SDK::Level::getHitResult() {
	int index = 0;
	switch (SDK::internalVers) {
	case V1_18_12:
		index = 0x139;
		break;
		//case V1_19_41:
	case V1_19_51:
		index = 0x148;
		break;
	default:
		index = 0x132;
		break;
	}

	return memory::callVirtual<HitResult*>(this, index);
}
