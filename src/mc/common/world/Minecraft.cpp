#include "pch.h"
#include "Minecraft.h"

SDK::Level* SDK::Minecraft::getLevel() {
    if (gameSession) {
        return reinterpret_cast<Level*>(gameSession->level);
    }
    return nullptr;
}
