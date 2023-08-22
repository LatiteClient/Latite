#pragma once
#include "sdk/common/world/actor/Mob.h"
#include "sdk/common/entity/component/MoveInputComponent.h"
#include "PlayerInventory.h"
#include <string>

namespace SDK {
	class Player : public Mob {
	public:
		void displayClientMessage(std::string const& message);

		MoveInputComponent* getMoveInputComponent();

		// xref: Player::getSupplies
		MVCLASS_FIELD(PlayerInventory*, supplies, 0x7C0, 0xB70, 0xDB8);
	};
}