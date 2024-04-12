#pragma once
#include "sdk/common/world/actor/Mob.h"
#include "sdk/common/entity/component/MoveInputComponent.h"
#include "PlayerInventory.h"
#include "sdk/common/network/PacketSender.h"
#include "GameMode.h"
#include <string>

namespace SDK {
	class Player : public Mob {
	public:
		void displayClientMessage(std::string const& message);

		MoveInputComponent* getMoveInputComponent();
		std::string getXUID();

		// @dump-wbds Player::getSupplies, 3
		MVCLASS_FIELD(PlayerInventory*, supplies, 0x7B0, 0x7C0, 0x7C0, 0xB70, 0xDB8); // xref: getSupplies
		// @dump-wbds Player::getGameMode, 3
		MVCLASS_FIELD(GameMode*, gameMode, 0xED8, 0xEB0, 0xE70, 0x1238, 0x1670); // xref: getGameMode
		// @dump-wbds Player::getName, 3
		MVCLASS_FIELD(std::string, playerName, 0x1D70, 0x1CB8, 0x1C78, 0x8C0, 0x2478); // xref: getName
		// @dump-wbds ServerPlayer::sendNetworkPacket, 18
		MVCLASS_FIELD(PacketSender*, packetSender, 0xC98, 0xC70, 0xC70, 0x1020, 0x1438); // xref: Player::Player a3
	};
}