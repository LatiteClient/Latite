#pragma once
#include "util/LMath.h"
#include "../actor/Actor.h"
#include <string>
#include "sdk/Util.h"
#include <unordered_map>

#include "LevelData.h"

namespace SDK {

	class PlayerListEntry {
		char pad[0x2D2]; // constructor
	public:
		CLASS_FIELD(std::string, name, 0x18); // constructor
	};

	class Level {
	public:
		void playSoundEvent(std::string const& text, Vec3 const& pos, float vol = 1.f, float pitch = 1.f);
		std::vector<SDK::Actor*> getRuntimeActorList();
		std::unordered_map<UUID, PlayerListEntry>* getPlayerList();
		class HitResult* getHitResult();
		class HitResult* getLiquidHitResult();
		bool isClientSide();

		MVCLASS_FIELD(std::shared_ptr<LevelData>, levelData, 0x100, 0x110, 0x110, 0x110, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8);
		MVCLASS_FIELD(std::string, name, 0x290, 0x6C8, 0x698, 0x678, 0x5B8, 0x548);

		const std::string& getLevelName();
	};
}
