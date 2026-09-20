#include "pch.h"
#include "MinecraftGame.h"
#include "mc/Addresses.h"

namespace {
    struct ClientInterface {
        void* clientInterface;
        std::shared_ptr<SDK::ClientInstance> clientInstance;
    };

    static_assert(offsetof(ClientInterface, clientInstance) == 0x8);
    static_assert(sizeof(ClientInterface) == 0x18);
}

bool SDK::MinecraftGame::isCursorGrabbed() {
    return hat::member_at<bool>(this, 0x1E8);
}

SDK::FontRepository* SDK::MinecraftGame::getFontRepository() {
    NonOwnerPointer<const FontRepository> fontRepository;
    memory::callVirtual<void, NonOwnerPointer<const FontRepository>&>(this, 0x3F, fontRepository);
    return const_cast<FontRepository*>(fontRepository.get());
}

SDK::ClientInstance* SDK::MinecraftGame::getPrimaryClientInstance() {
    const auto& clients = hat::member_at<std::map<uint8_t, ClientInterface>>(this, 0x970);
    const auto primary = clients.find(0);
    return primary != clients.end() ? primary->second.clientInstance.get() : nullptr;
}
