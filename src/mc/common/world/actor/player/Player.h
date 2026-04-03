#pragma once
#include "mc/common/world/actor/Mob.h"
#include "mc/common/entity/component/MoveInputComponent.h"
#include "PlayerInventory.h"
#include "mc/common/network/PacketSender.h"
#include "GameMode.h"
#include <string>

namespace SDK {
	class Player : public Mob {
	public:
		void displayClientMessage(std::string const& message);

		MoveInputComponent* getMoveInputComponent();
		std::string getXUID();

		// @dump-wbds Player::getSupplies, 3
		CLASS_FIELD(PlayerInventory*, supplies, 0x5B8); // xref: getSupplies
		// @dump-wbds Player::getGameMode, 3
		CLASS_FIELD(GameMode*, gameMode, 0xAA0); // xref: getGameMode
		// @dump-wbds Player::getName, 3
		CLASS_FIELD(std::string, playerName, 0xBC0); // xref: getName
		// @dump-wbds ServerPlayer::sendNetworkPacket, 18
		CLASS_FIELD(PacketSender*, packetSender, 0x7F8); // xref: Player::Player a3
	};
}