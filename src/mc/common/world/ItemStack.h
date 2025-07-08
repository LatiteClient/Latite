#pragma once
#include "ItemStackBase.h"

namespace SDK {
	class ItemStack : public ItemStackBase {
	private:
		uintptr_t netIds;
	};
}