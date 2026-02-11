#pragma once
#include "mc/deps/core/StringUtils.h"
#include "mc/Util.h"

namespace SDK {
	class BlockLegacy {
	public:
		CLASS_FIELD(std::string, translateName, 0x8); // tile.deepslate_diamond_ore
		CLASS_FIELD(StringHash, name, 0x78); // deepslate_diamond_ore
		CLASS_FIELD(std::string, Namespace, 0xA8); // minecraft
		CLASS_FIELD(StringHash, namespacedId, 0xC8); // minecraft:deepslate_diamond_ore
		CLASS_FIELD(std::string, itemGroup, 0x160); // itemGroup.name.ore
	private:
		virtual ~BlockLegacy() = 0;
	};
}