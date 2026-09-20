#include "Platform_GameCore.h"

SDK::MinecraftGame* SDK::Platform_GameCore::getMinecraftGame() {
    const auto gameHolder = hat::member_at<void*>(this, 0x118);
    return gameHolder ? hat::member_at<MinecraftGame*>(gameHolder, 0x48) : nullptr;
}

SDK::GameCore* SDK::Platform_GameCore::getGameCore() {
    return hat::member_at<GameCore*>(this, 0x78);
}

SDK::Platform_GameCore* SDK::Platform_GameCore::get() {
    const auto winMain = *reinterpret_cast<void**>(Signatures::Misc::Platform_GameCore.result);
    return winMain ? hat::member_at<Platform_GameCore*>(winMain, 0x8) : nullptr;
}
