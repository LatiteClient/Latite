#pragma once

#include "GameCore.h"
#include "MinecraftGame.h"

namespace SDK {
    class Platform_GameCore {
    public:
        MinecraftGame* getMinecraftGame();
        GameCore* getGameCore();

        static Platform_GameCore* get();
    };
}
