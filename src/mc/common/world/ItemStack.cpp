#include "pch.h"

#include <cstring>

#include "ItemStack.h"
#include "mc/common/world/level/block/Block.h"

namespace {
    constexpr std::size_t BlockTypeAsItemInstanceVtableIndex = 0x4D; // BlockType::asItemInstance in 26.50
}

SDK::ItemStack* SDK::ItemStack::constructBlockItem(void* storage, SDK::Block const& block) {
    auto blockType = block.getBlockType();
    if (!storage || !blockType || !Signatures::ItemStackVtable.result) return nullptr;

    using AsItemInstance =
        ItemStackBase*(__fastcall*)(BlockLegacy const*, ItemStackBase*, Block const*, BlockActor const*);
    auto blockTypeVtable = *reinterpret_cast<void***>(blockType);
    auto asItemInstance = reinterpret_cast<AsItemInstance>(blockTypeVtable[BlockTypeAsItemInstanceVtableIndex]);

    std::memset(storage, 0, sizeof(ItemStack));
    auto item = asItemInstance(blockType, static_cast<ItemStackBase*>(storage), &block, nullptr);
    if (!item) return nullptr;

    item->vtable = reinterpret_cast<void**>(Signatures::ItemStackVtable.result);

    return reinterpret_cast<ItemStack*>(item);
}

void SDK::ItemStack::destruct() {
    using oFunc_t = void(__fastcall*)(ItemStackBase*);
    auto fn = reinterpret_cast<oFunc_t>(Signatures::ItemStackBase_destructor.result);
    if (fn) {
        fn(this);
    }
}
