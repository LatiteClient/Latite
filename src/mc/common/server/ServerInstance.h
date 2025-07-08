#pragma once
#include "mc/Util.h"

namespace SDK {
	class ServerInstance {
		void* vtable;
	public:
		CLASS_FIELD(class Minecraft*, minecraft, 0xB8);
	};
}