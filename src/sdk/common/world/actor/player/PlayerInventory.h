#pragma once
#include "sdk/Util.h"
#include "Inventory.h"

namespace SDK {
	class PlayerInventory {
	public:
		CLASS_FIELD(int, selectedSlot, 0x10);
		MVCLASS_FIELD(Inventory*, inventory, 0xC0, 0xB0, 0xC0);
	};
}