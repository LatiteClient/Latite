#include "pch.h"
#include "MinecraftGame.h"
#include "mc/Addresses.h"

namespace {
    struct ClientMapNode {
        ClientMapNode* left;
        ClientMapNode* parent;
        ClientMapNode* right;
        uint8_t color;
        bool isNil;
        std::byte nodePadding[6];
        uint8_t subClientId;
        std::byte valuePadding[15];
        SDK::ClientInstance* clientInstance;
    };

    static_assert(offsetof(ClientMapNode, subClientId) == 0x20);
    static_assert(offsetof(ClientMapNode, clientInstance) == 0x30);
}

bool SDK::MinecraftGame::isCursorGrabbed() {
    return memory::callVirtual<bool>(this, 0x63);
}

SDK::FontRepository* SDK::MinecraftGame::getFontRepository() {
    NonOwnerPointer<const FontRepository> fontRepository;
    memory::callVirtual<void, NonOwnerPointer<const FontRepository>&>(this, 0xAF, fontRepository);
    return const_cast<FontRepository*>(fontRepository.get());
}

SDK::ClientInstance* SDK::MinecraftGame::getPrimaryClientInstance() {
    const auto head = hat::member_at<ClientMapNode*>(this, 0x970);
    if (!head || !head->parent) {
        return nullptr;
    }

    constexpr uint8_t primarySubClientId = 0;
    auto candidate = head;
    auto node = head->parent;

    while (!node->isNil) {
        if (node->subClientId < primarySubClientId) {
            node = node->right;
        } else {
            candidate = node;
            node = node->left;
        }
    }

    if (candidate == head || candidate->subClientId != primarySubClientId) {
        return nullptr;
    }

    return candidate->clientInstance;
}
