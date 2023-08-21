#include "MinecraftGame.h"
#include "sdk/signature/storage.h"

bool SDK::MinecraftGame::isCursorGrabbed() {
    if (SDK::internalVers == VLATEST)
        return util::directAccess<bool>(this, Signatures::Offset::MinecraftGame_cursorGrabbed.result);
    // v1_18_12
    if (SDK::internalVers == V1_19_51) return util::directAccess<bool>(this, 0xD8);
    return util::directAccess<bool>(this, 0x318);
}
