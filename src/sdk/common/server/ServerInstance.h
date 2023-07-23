#pragma once
#include "util/Util.h"
#include "sdk/common/world/Minecraft.h"

namespace sdk {
	class ServerInstance {
		void* vtable;
	public:
		CLASS_FIELD(Minecraft*, minecraft, 0xA8);
	};
}