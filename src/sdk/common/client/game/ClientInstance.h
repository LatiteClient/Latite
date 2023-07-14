#pragma once
#include "util/util.h"

namespace sdk {
	class ClientInstance {
	public:
		CLASS_FIELD(class MinecraftGame*, minecraftGame, 0xC8);
		CLASS_FIELD(class Minecraft*, minecraft, 0xD0);
		//CLASS_FIELD(Vec2, cursorPos, 0x498);
		CLASS_FIELD(class LevelRenderer*, levelRenderer, 0xE0);

		static ClientInstance* get();
	};
}