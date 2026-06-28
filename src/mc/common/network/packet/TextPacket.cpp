#include "pch.h"
#include "TextPacket.h"
#include "mc/common/client/game/ClientInstance.h"
#include "mc/common/client/game/MinecraftGame.h"
#include "mc/common/client/player/LocalPlayer.h"
#include "mc/Addresses.h"
#include "util/memory.h"

uint8_t SDK::TextPacket::getBodyVariant() const {
    return *(reinterpret_cast<const uint8_t*>(&this->data) + 0x20);
}

std::string* SDK::TextPacket::getMessage() {
    switch (this->getBodyVariant()) {
    case 0: // message only variant
    case 2: // message and params variant
        return &this->str;
    case 1: // author and message variant
        return reinterpret_cast<std::string*>(&this->data);
    default:
        return nullptr;
    }
}

const std::string* SDK::TextPacket::getMessage() const {
    return const_cast<TextPacket*>(this)->getMessage();
}

std::string* SDK::TextPacket::getAuthor() {
    return this->getBodyVariant() == 1 ? &this->str : nullptr;
}

const std::string* SDK::TextPacket::getAuthor() const {
    return const_cast<TextPacket*>(this)->getAuthor();
}

void SDK::TextPacket::chat(std::string const& message) {
    this->data = message;
    this->str = SDK::ClientInstance::get()->getLocalPlayer()->playerName;
    this->xboxUserId = SDK::ClientInstance::get()->minecraftGame->xuid;
    this->type = TextPacketType::CHAT;
}
