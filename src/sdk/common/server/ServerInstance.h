#pragma once

#include <util/Util.h>

#include "Minecraft.h"

namespace sdk {
	class ServerInstance {
		void* vtable;
	public:
		CLASS_FIELD(sdk::Minecraft*, minecraft, 0xA8);
	};
}