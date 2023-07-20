#pragma once
#include "sdk/common/world/actor/Mob.h"
#include <string>

namespace sdk {
	class Player : public Mob {
	public:
		void displayClientMessage(std::string const& message);
	};
}