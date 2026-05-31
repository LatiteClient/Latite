#pragma once

#include "mc/Util.h"
#include "mc/common/world/ItemStack.h"
#include "mc/common/world/actor/Actor.h"

namespace SDK {
	class ItemActor : public Actor {
	public:
		CLASS_FIELD(ItemStack, itemStack, 0x3B0);

		ItemStack* getItemStack() {
			return &itemStack;
		}
	};
}
