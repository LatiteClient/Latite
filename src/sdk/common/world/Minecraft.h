#pragma once
#include "level/Level.h"
#include "sdk/Util.h"
#include "Timer.h"

namespace sdk {
	class Minecraft {
	public:
		Level* getLevel();

		CLASS_FIELD(class GameSession*, gameSession, 0xC0);
		CLASS_FIELD(Timer*, timer, 0xD8);
	};

	class GameSession {
	public:
		CLASS_FIELD(Level**, level, 0x20);
	};
}