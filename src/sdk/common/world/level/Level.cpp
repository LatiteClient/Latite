#include "pch.h"
#include "Level.h"

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
		index = 0xA3;
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
		index = 0x125;
		break;
	}

	memory::callVirtual<void, std::vector<Actor*>&>(this, index, list);
	return list;
}

std::unordered_map<UUID, SDK::PlayerListEntry>* SDK::Level::getPlayerList() {
	return memory::callVirtual<std::unordered_map<UUID, SDK::PlayerListEntry>*>(this, SDK::mvGetOffset<0x120, 0x128, 0x137>());
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
		index = 0x12E;
		break;
	}

	return memory::callVirtual<HitResult*>(this, index);
}

SDK::HitResult* SDK::Level::getLiquidHitResult() {
	int index = 0;
	switch (SDK::internalVers) {
	case V1_18_12:
		index = 0x13A;
		break;
		//case V1_19_41:
	case V1_19_51:
		index = 0x149;
		break;
	default:
		index = 0x12F;
		break;
	}

	return reinterpret_cast<SDK::HitResult*>(memory::callVirtual<uintptr_t>(this, index)) /*sizeof hitResult (0x60) / 8*/;
}

bool SDK::Level::isClientSide() {
	return memory::callVirtual<bool>(this, SDK::mvGetOffset<0x11F, 0x12B, 0x127>());
}
