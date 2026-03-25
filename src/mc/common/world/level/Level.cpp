#include "pch.h"
#include "Level.h"

void SDK::Level::playSoundEvent(std::string const& text, Vec3 const& pos, float vol, float pitch) {
	memory::callVirtual<void>(this, 0xBA, text, pos, vol, pitch);
}

std::vector<SDK::Actor*> SDK::Level::getRuntimeActorList() {
	std::vector<Actor*> list;

	// TODO: this might return a vector too?
	memory::callVirtual<void, std::vector<Actor*>&>(this, 0x142, list);
	return list;
}

std::unordered_map<UUID, SDK::PlayerListEntry>* SDK::Level::getPlayerList() {
	return *reinterpret_cast<std::unordered_map<UUID, SDK::PlayerListEntry>**>(reinterpret_cast<uintptr_t>(this) + 0x4E0);
}

SDK::HitResult* SDK::Level::getHitResult() {
	return memory::callVirtual<HitResult*>(this, 0x14C);
}

SDK::HitResult* SDK::Level::getLiquidHitResult() {
	return reinterpret_cast<SDK::HitResult*>(memory::callVirtual<uintptr_t>(this, 0x14D)) /*sizeof hitResult (0x60) / 8*/;
}

bool SDK::Level::isClientSide() {
	return memory::callVirtual<bool>(this, 0x13B);
}

const std::string& SDK::Level::getLevelName() {
	return levelData->levelName;
}
