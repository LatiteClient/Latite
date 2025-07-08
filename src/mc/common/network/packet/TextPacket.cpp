#include "pch.h"
#include "TextPacket.h"
#include "mc/common/client/game/ClientInstance.h"
#include "mc/common/client/game/MinecraftGame.h"
#include "mc/common/client/player/LocalPlayer.h"
#include "mc/Addresses.h"
#include "util/memory.h"

void SDK::TextPacket::chat(std::string const& message) {
	this->str = message;
	this->source = SDK::ClientInstance::get()->getLocalPlayer()->playerName;
	this->xboxUserId = SDK::ClientInstance::get()->minecraftGame->xuid;
	this->type = TextPacketType::CHAT;
}
