#include "pch.h"
#include "MinecraftGame.h"
#include "mc/Addresses.h"

bool SDK::MinecraftGame::isCursorGrabbed() {
    return hat::member_at<bool>(this, 0x1D8);
}

SDK::ClientInstance* SDK::MinecraftGame::getPrimaryClientInstance() {
    const auto map = hat::member_at<std::map<uint8_t, std::shared_ptr<ClientInstance>>>(this, 0x908);
    return map.at(0).get();
}
