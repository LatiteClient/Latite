#pragma once
#include "sdk/Util.h"
#include "api/memory/memory.h"
#include <string>

namespace SDK {
	class MinecraftGame {
	public:
		bool isCursorGrabbed();

		MVCLASS_FIELD(std::string, xuid, 0x2F8, 0x618, 0x228);
	private:
		MVCLASS_FIELD(class FontRepository**, fontRepo, 0xF20, 0x110, 0xF88);
	public:
		MVCLASS_FIELD(class Font*, minecraftFont, 0xF30, 0x120, 0xF78);

		class FontRepository* getFontRepository() {
			if (internalVers > V1_19_41) {
				return *fontRepo;
			}
			return reinterpret_cast<FontRepository*>(fontRepo);
		}

		class ClientInstance* getPrimaryClientInstance() {
			return memory::callVirtual<ClientInstance*>(this, 0x2);
		}
	};
}