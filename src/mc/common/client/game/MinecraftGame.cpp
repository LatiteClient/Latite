#include "pch.h"
#include "MinecraftGame.h"
#include "mc/Addresses.h"

bool SDK::MinecraftGame::isCursorGrabbed() {
    return hat::member_at<bool>(this, 0x1D8);
}

SDK::ClientInstance* SDK::MinecraftGame::getPrimaryClientInstance() {
    const auto& map = hat::member_at<std::map<uint8_t, std::shared_ptr<ClientInstance>>>(this, 0x938);
    const auto primary = map.find(0);
    return primary != map.end() ? primary->second.get() : nullptr;
}
