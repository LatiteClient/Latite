#pragma once
#include "mc/Util.h"
#include "util/memory.h"
#include <string>

#include "mc/common/client/renderer/GameRenderer.h"

namespace SDK {
	class MinecraftGame {
	public:
		bool isCursorGrabbed();

		MVCLASS_FIELD(std::string, xuid, 0x358, 0x350, 0x350, 0x350, 0x350, 0x350, 0x320, 0x310, 0x310, 0x310, 0x310, 0x320, 0x318, 0x318, 0x2F8, 0x618, 0x228);
		CLASS_FIELD(GameRenderer*, gameRenderer, 0xD70);
	private:
		MVCLASS_FIELD(class FontRepository**, fontRepo, 0xF10, 0xED8, 0xED0, 0xEE0, 0xEC0, 0xEC0, 0xE70, 0xE38, 0xE08, 0xEB0, 0xDE0, 0xE08, 0xE00, 0xF60, 0xF30, 0xF20, 0x110, 0xF88);
	public:
		MVCLASS_FIELD(class Font*, minecraftFont, 0xF18, 0xEE0, 0xED8, 0xEE8, 0xEC8, 0xEC8, 0xE78, 0xE40, 0xE10, 0xEB8, 0xDE8, 0xE18, 0xE10, 0xF70, 0xF40, 0xF30, 0x120, 0xF78);

		class FontRepository* getFontRepository() {
			if (internalVers >= V1_21_40) {
				return reinterpret_cast<FontRepository*>(fontRepo);
			}
			if (internalVers > V1_19_41) {
				return *fontRepo;
			}
			return reinterpret_cast<FontRepository*>(fontRepo);
		}

		class ClientInstance* getPrimaryClientInstance();
	};
}
