#include "pch.h"
#include "MinecraftGame.h"
#include "mc/Addresses.h"

bool SDK::MinecraftGame::isCursorGrabbed() {
    return hat::member_at<bool>(this, Signatures::Offset::MinecraftGame_cursorGrabbed.result);
}

SDK::ClientInstance* SDK::MinecraftGame::getPrimaryClientInstance() {
    const auto map = reinterpret_cast<std::map<unsigned char, std::shared_ptr<SDK::ClientInstance>>*>(reinterpret_cast<uintptr_t>(this) + 0xA08);
    return map->at(0).get();
}
