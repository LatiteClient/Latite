#pragma once
#include "mc/Util.h"
#include <string>

namespace SDK {
	class Dimension {
	public:
		CLASS_FIELD(std::string, dimensionName, 0x20); // reclass
		CLASS_FIELD(class BlockSource*, region, 0xF0); // xref: Actor::getBlockSource something like that
		CLASS_FIELD(class Weather*, weather, 0x1C0);
	};
}