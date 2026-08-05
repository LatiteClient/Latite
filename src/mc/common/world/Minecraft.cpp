#include "pch.h"
#include "Minecraft.h"

SDK::Level* SDK::Minecraft::getLevel() {
    auto* session = gameSession;
    return session ? session->getLevel() : nullptr;
}

SDK::Level* SDK::GameSession::getLevel() {
    if (hat::member_at<uint8_t>(this, 0x28) != 1) return nullptr;

    auto* levelControlBlock = hat::member_at<uint8_t*>(this, 0x30);
    if (!levelControlBlock || *levelControlBlock != 1) return nullptr;

    return hat::member_at<Level*>(this, 0x40);
}
