#pragma once
#include <cstddef>

#include "ItemStackBase.h"

namespace SDK {
	class ItemStack : public ItemStackBase {
	private:
		std::byte netIds[0x18]{};

	public:
		static ItemStack *constructFromBlock(void *storage, class Block const &block, int count = 1,
		                                     class CompoundTag const *userData = nullptr);

		void destruct();
	};

	static_assert(sizeof(ItemStack) == 0x98);
}
