#pragma once
#include "util/Util.h"
#include "sdk/common/world/Minecraft.h"

namespace SDK {
	class ServerInstance {
		void* vtable;
	public:
		CLASS_FIELD(Minecraft*, minecraft, 0xA8);
	};
}