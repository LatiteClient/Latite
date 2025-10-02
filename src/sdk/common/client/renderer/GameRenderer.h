#pragma once
#include "MinecraftMatrix.h"

namespace SDK {
    class GameRenderer {
        char pad[0x340];
    
    public:
        SDK::MinecraftMatrix lastViewMatrix;

    private:
        char pad2[0x40];

    public:
        SDK::MinecraftMatrix lastProjectionMatrix;
    };
}
