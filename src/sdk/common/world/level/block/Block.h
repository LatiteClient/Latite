#pragma once
#include "sdk/Util.h"
#include "BlockLegacy.h"

namespace SDK {
	class Block {
	public:
		MVCLASS_FIELD(BlockLegacy*, legacyBlock, 0x78, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x10, 0x10);
	
		virtual ~Block() = 0;
		virtual int getRenderLayer() const = 0;
	};
}