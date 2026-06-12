#pragma once

#include "Item.h"
#include "ItemTier.h"

namespace SDK {
	class DiggerItem : public Item {
	public:
		CLASS_FIELD(ItemTier const*, tier, 0x210);
	};
}
