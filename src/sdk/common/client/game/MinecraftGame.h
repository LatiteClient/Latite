#pragma once
#include "sdk/Util.h"
#include "api/memory/memory.h"
#include <string>

namespace SDK {
	class MinecraftGame {
	public:
		bool isCursorGrabbed();

		MVCLASS_FIELD(std::string, xuid, 0x2F0, 0x618, 0x228);
		MVCLASS_FIELD(class Font*, minecraftFont, 0xEC0, 0xF78, 0xF78);

		class ClientInstance* getPrimaryClientInstance() {
			return memory::callVirtual<ClientInstance*>(this, 0x2);
		}
	};
}