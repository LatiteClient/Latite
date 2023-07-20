#include "MinecraftGame.h"
#include "client/signature/storage.h"

bool sdk::MinecraftGame::isMouseGrabbed() {
    return util::directAccess<bool>(this, Signatures::Offset::MinecraftGame_cursorGrabbed.result);
}
