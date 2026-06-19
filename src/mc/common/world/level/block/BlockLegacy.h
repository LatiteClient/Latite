#pragma once
#include "mc/deps/core/StringUtils.h"
#include "mc/Util.h"

namespace SDK {
	class BlockLegacy {
	public:
		CLASS_FIELD(std::string, translateName, 0x8); // tile.deepslate_diamond_ore
		CLASS_FIELD(HashedString, name, 0x90); // deepslate_diamond_ore
		CLASS_FIELD(std::string, Namespace, 0xC0); // minecraft
		CLASS_FIELD(HashedString, namespacedId, 0xE0); // minecraft:deepslate_diamond_ore
	private:
		virtual ~BlockLegacy() = 0;
	};
}
