#pragma once
#include "Minecraft.h"

namespace mc {
	class ServerInstance {
		void* vtable;
	public:
		CLASS_FIELD(Minecraft*, minecraft, 0xA8);
	};
}