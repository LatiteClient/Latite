#include "MinecraftGame.h"
#include "sdk/signature/storage.h"

bool sdk::MinecraftGame::isCursorGrabbed() {
    return util::directAccess<bool>(this, Signatures::Offset::MinecraftGame_cursorGrabbed.result);
}
