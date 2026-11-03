#include "pch.h"
#include "MinecraftGame.h"
#include "mc/Addresses.h"

namespace {
    struct ClientInterface {
        void* clientInterface;
        void* unk;
        std::shared_ptr<SDK::ClientInstance> clientInstance;
    };

    static_assert(offsetof(ClientInterface, clientInstance) == 0x10);
    static_assert(sizeof(ClientInterface) == 0x20);
}

bool SDK::MinecraftGame::isCursorGrabbed() {
    return hat::member_at<bool>(this, 0x1F8);
}

SDK::FontRepository* SDK::MinecraftGame::getFontRepository() {
    return hat::member_at<FontRepository*>(this, 0x758);
}

SDK::ClientInstance* SDK::MinecraftGame::getPrimaryClientInstance() {
    const auto& clients = hat::member_at<std::map<uint8_t, ClientInterface>>(this, 0x9D0);
    const auto primary = clients.find(0);
    return primary != clients.end() ? primary->second.clientInstance.get() : nullptr;
}
