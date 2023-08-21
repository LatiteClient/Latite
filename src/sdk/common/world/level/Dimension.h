#pragma once
#include "sdk/Util.h"
#include <string>

namespace SDK {
	class Dimension {
	public:
		CLASS_FIELD(std::string, dimensionName, 0x20); // reclass
		CLASS_FIELD(class BlockSource*, region, 0xC0); // xref: Actor::getBlockSource something like that
		CLASS_FIELD(class VillageManager*, villageManager, 0x588); // xref: getVillageManager
	};
}