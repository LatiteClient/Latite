#pragma once
#include "mc/Util.h"
#include "BlockLegacy.h"

namespace SDK {
	class Block {
	public:
		CLASS_FIELD(BlockLegacy*, legacyBlock, 0x58);
	
		virtual ~Block() = 0;
		virtual int getRenderLayer() const = 0;
	};
}