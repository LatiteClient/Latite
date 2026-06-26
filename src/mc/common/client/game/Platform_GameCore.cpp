#include "Platform_GameCore.h"

SDK::MinecraftGame* SDK::Platform_GameCore::getMinecraftGame() {
    return hat::member_at<MinecraftGame*>(this, 0x38);
}

SDK::GameCore* SDK::Platform_GameCore::getGameCore() {
    return hat::member_at<GameCore*>(this, 0xD0);
}

SDK::Platform_GameCore* SDK::Platform_GameCore::get() {
    static auto ptr =
        hat::member_at<Platform_GameCore*>(*reinterpret_cast<void**>(Signatures::Misc::Platform_GameCore.result), 0x8);
    return ptr;
}
