#pragma once
#include "util/LMath.h"
#include "../actor/Actor.h"
#include "mc/common/world/actor/player/PlayerListEntry.h"
#include <string>
#include "mc/Util.h"
#include <unordered_map>

#include "LevelData.h"

namespace SDK {
    class Level {
    public:
        void playSoundEvent(std::string const& text, Vec3 const& pos, float vol = 1.f, float pitch = 1.f);
        std::vector<SDK::Actor*> getRuntimeActorList();
        std::unordered_map<UUID, PlayerListEntry>* getPlayerList();
        class HitResult* getHitResult();
        class HitResult* getLiquidHitResult();
        bool isClientSide();

        CLASS_FIELD(std::shared_ptr<LevelData>, levelData, 0x90);
        CLASS_FIELD(std::string, name, 0x290);

        const std::string& getLevelName();
    };
}
