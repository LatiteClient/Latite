#include "Platform_GameCore.h"

SDK::MinecraftGame* SDK::Platform_GameCore::getMinecraftGame() {
    return hat::member_at<MinecraftGame*>(this, 0x18);
}

SDK::GameCore* SDK::Platform_GameCore::getGameCore() {
    return hat::member_at<GameCore*>(this, 0xB0);
}

SDK::Platform_GameCore* SDK::Platform_GameCore::get() {
    const auto winMain = *reinterpret_cast<void**>(Signatures::Misc::Platform_GameCore.result);
    return winMain ? hat::member_at<Platform_GameCore*>(winMain, 0x8) : nullptr;
}
