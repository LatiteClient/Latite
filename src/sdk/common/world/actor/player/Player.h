#pragma once
#include "sdk/common/world/actor/Mob.h"
#include "sdk/common/entity/component/MoveInputComponent.h"
#include <string>

namespace SDK {
	class Player : public Mob {
	public:
		void displayClientMessage(std::string const& message);

		MoveInputComponent* getMoveInputComponent();
	};
}