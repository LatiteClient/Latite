#pragma once
#include "sdk/deps/Core/StringUtils.h"
#include "sdk/Util.h"

namespace SDK {
	class BlockLegacy {
	public:
		CLASS_FIELD(std::string, translateName, 0x8); // tile.deepslate_diamond_ore
		CLASS_FIELD(FNVString, name, 0x90); // deepslate_diamond_ore
		CLASS_FIELD(std::string, Namespace, 0xC0); // minecraft
		CLASS_FIELD(FNVString, namespacedId, 0xE0); // minecraft:deepslate_diamond_ore
		CLASS_FIELD(std::string, itemGroup, 0x178); // itemGroup.name.ore
	private:
		virtual ~BlockLegacy() = 0;
	};
}