#include "pch.h"
#include "Level.h"

void SDK::Level::playSoundEvent(std::string const& text, Vec3 const& pos, float vol, float pitch) {
	static int index = mvGetOffset<0xA2, 0xA3, 0xA3, 0xBD, 0xC9>();
	memory::callVirtual<void>(this, index, text, pos, vol, pitch);
}

std::vector<SDK::Actor*> SDK::Level::getRuntimeActorList() {
	std::vector<Actor*> list;
	static int index = mvGetOffset<0x117, 0x125, 0x125, 0x12D, 0x13C>();

	// TODO: this might return a vector too?
	memory::callVirtual<void, std::vector<Actor*>&>(this, index, list);
	return list;
}

std::unordered_map<UUID, SDK::PlayerListEntry>* SDK::Level::getPlayerList() {
	static int index = SDK::mvGetOffset<0x112, 0x120, 0x120, 0x128, 0x137>();
	return memory::callVirtual<std::unordered_map<UUID, SDK::PlayerListEntry>*>(this, index);
}

SDK::HitResult* SDK::Level::getHitResult() {
	static int index = mvGetOffset<0x121, 0x12E, 0x12E, 0x139, 0x148>();
	return memory::callVirtual<HitResult*>(this, index);
}

SDK::HitResult* SDK::Level::getLiquidHitResult() {
	static int index = mvGetOffset<0x122, 0x12F, 0x12F, 0x13A, 0x149>();
	return reinterpret_cast<SDK::HitResult*>(memory::callVirtual<uintptr_t>(this, index)) /*sizeof hitResult (0x60) / 8*/;
}

bool SDK::Level::isClientSide() {
	return memory::callVirtual<bool>(this, SDK::mvGetOffset<0x111, 0x11F, 0x11F, 0x12B, 0x127>());
}
