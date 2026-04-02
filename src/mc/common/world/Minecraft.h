#pragma once
#include "level/Level.h"
#include "mc/Util.h"
#include "Timer.h"

namespace SDK {
	class Minecraft {
	public:
		Level* getLevel();

		CLASS_FIELD(class GameSession*, gameSession, 0xB8);
		CLASS_FIELD(Timer*, timer, 0xD0);
	};

	class GameSession {
	public:
		CLASS_FIELD(Level**, level, 0x40);
	};
}