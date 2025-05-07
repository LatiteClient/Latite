#pragma once
#include "level/Level.h"
#include "sdk/Util.h"
#include "Timer.h"

namespace SDK {
	class Minecraft {
	public:
		Level* getLevel();

		CLASS_FIELD(class GameSession*, gameSession, 0xD0);
		CLASS_FIELD(Timer*, timer, 0xE8);
	};

	class GameSession {
	public:
		MVCLASS_FIELD(Level**, level, 0x40, 0x30, 0x30, 0x30, 0x28 /*1.20.40*/, 0x20, 0x20, 0x20);
	};
}