#pragma once
#include "mc/Util.h"
#include "Inventory.h"

namespace SDK {
	class PlayerInventory {
	public:
		CLASS_FIELD(int, selectedSlot, 0x10);
		CLASS_FIELD(Inventory*, inventory, 0xB8);
	};
}