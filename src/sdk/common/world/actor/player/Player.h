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

		MVCLASS_FIELD(PlayerInventory*, supplies, 0x7E8, 0x7C0, 0x7C0, 0xB70, 0xDB8); // xref: getSupplies
		MVCLASS_FIELD(GameMode*, gameMode, 0xF10, 0xEB0, 0xE70, 0x1238, 0x1670); // xref: getGameMode
		MVCLASS_FIELD(std::string, playerName, 0x1D28, 0x1CB8, 0x1C78, 0x8C0, 0x2478); // xref: getName
		MVCLASS_FIELD(PacketSender*, packetSender, 0xCD0, 0xC70, 0xC70, 0x1020, 0x1438); // xref: Player::Player a3
	};
}