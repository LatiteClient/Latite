#include "pch.h"
#include "Level.h"

void SDK::Level::playSoundEvent(std::string const& text, Vec3 const& pos, float vol, float pitch) {
	static int index = mvGetOffset<0xB2, 0xB3, 0xB2, 0xB2, 0xA2, 0xA2, 0xA2, 0xA3, 0xA3, 0xBD, 0xC9>();
	memory::callVirtual<void>(this, 0xB2, text, pos, vol, pitch);
}

std::vector<SDK::Actor*> SDK::Level::getRuntimeActorList() {
	std::vector<Actor*> list;
	static int index = mvGetOffset<0x134, 0x136, 0x134, 0x132, 0x134, 0x117, 0x117, 0x116, 0x125, 0x125, 0x12D, 0x13C>();

	// TODO: this might return a vector too?
	memory::callVirtual<void, std::vector<Actor*>&>(this, index, list);
	return list;
}

std::unordered_map<UUID, SDK::PlayerListEntry>* SDK::Level::getPlayerList() {
	if (internalVers >= SDK::V1_21_40) {
		return *reinterpret_cast<std::unordered_map<UUID, SDK::PlayerListEntry>**>(reinterpret_cast<uintptr_t>(this) + 0x950);
	}
	
	static int index = SDK::mvGetOffset<0x112, 0x111, 0x120, 0x120, 0x128, 0x137>();
	return memory::callVirtual<std::unordered_map<UUID, SDK::PlayerListEntry>*>(this, index);
}

SDK::HitResult* SDK::Level::getHitResult() {
	static int index = mvGetOffset<0x13F, 0x140, 0x13E, 0x13C, 0x13E, 0x121, 0x121, 0x120, 0x12E, 0x12E, 0x139, 0x148>();
	return memory::callVirtual<HitResult*>(this, index);
}

SDK::HitResult* SDK::Level::getLiquidHitResult() {
	static int index = mvGetOffset<0x140, 0x141, 0x13F, 0x13D, 0x13F, 0x122, 0x122, 0x121, 0x12F, 0x12F, 0x13A, 0x149>();
	return reinterpret_cast<SDK::HitResult*>(memory::callVirtual<uintptr_t>(this, index)) /*sizeof hitResult (0x60) / 8*/;
}

bool SDK::Level::isClientSide() {
	return memory::callVirtual<bool>(this, SDK::mvGetOffset<0x12E, 0x12F, 0x12D, 0x12B, 0x12C, 0x111, 0x111, 0x110, 0x11F, 0x11F, 0x12B, 0x127>());
}

const std::string& SDK::Level::getLevelName() {
	if (internalVers >= V1_21_20) {
		return levelData->levelName;
	}
	return name;
}
