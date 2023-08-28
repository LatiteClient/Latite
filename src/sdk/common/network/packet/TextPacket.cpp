#include "pch.h"
#include "TextPacket.h"
#include "sdk/common/client/game/ClientInstance.h"
#include "sdk/common/client/game/MinecraftGame.h"
#include "sdk/common/client/player/LocalPlayer.h"
#include "sdk/signature/storage.h"
#include "api/memory/Memory.h"

SDK::TextPacket::TextPacket() {
	*reinterpret_cast<uintptr_t*>(this) = Signatures::Vtable::TextPacket.result;
}

void SDK::TextPacket::chat(String const& message) {
	this->str = message;
	this->source.setString(SDK::ClientInstance::get()->getLocalPlayer()->playerName.c_str());
	this->xboxUserId.setString(SDK::ClientInstance::get()->minecraftGame->xuid.c_str());
	this->type = TextPacketType::CHAT;
}
