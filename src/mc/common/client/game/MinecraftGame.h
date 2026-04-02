#pragma once
#include "mc/Util.h"
#include "util/memory.h"
#include <string>

#include "mc/common/client/renderer/GameRenderer.h"

namespace SDK {
	class MinecraftGame {
	public:
		bool isCursorGrabbed();

		CLASS_FIELD(std::string, xuid, 0x358);
		CLASS_FIELD(GameRenderer*, gameRenderer, 0xD78);
	private:
		CLASS_FIELD(class FontRepository**, fontRepo, 0xF18);
	public:
		CLASS_FIELD(class Font*, minecraftFont, 0xF20);

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
