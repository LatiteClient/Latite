#pragma once
#include "mc/deps/core/StringUtils.h"
#include "mc/Util.h"

namespace SDK {
	class BlockLegacy {
	public:
		CLASS_FIELD(std::string, translateName, 0x8); // tile.deepslate_diamond_ore
		CLASS_FIELD(HashedString, name, 0x88); // deepslate_diamond_ore
		CLASS_FIELD(std::string, Namespace, 0xB8); // minecraft
		CLASS_FIELD(HashedString, namespacedId, 0xD8); // minecraft:deepslate_diamond_ore
		CLASS_FIELD(std::string, itemGroup, 0x170); // itemGroup.name.ore
	private:
		virtual ~BlockLegacy() = 0;
	};
}
