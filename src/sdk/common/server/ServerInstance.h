#pragma once
#include "sdk/Util.h"

namespace SDK {
	class ServerInstance {
		void* vtable;
	public:
		CLASS_FIELD(class Minecraft*, minecraft, 0xB8);
	};
}