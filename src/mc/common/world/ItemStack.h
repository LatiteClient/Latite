#pragma once
#include <cstddef>

#include "ItemStackBase.h"

namespace SDK {
    class Block;
    class BlockActor;

    class ItemStack : public ItemStackBase {
    private:
        std::byte netIds[0x18] {};

    public:
        static ItemStack* constructBlockItem(void* storage, Block const& block);

        void destruct();
    };

    static_assert(sizeof(ItemStack) == 0x98);
}
