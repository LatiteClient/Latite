#pragma once
#include <chrono>
#include "Item.h"
#include "../nbt/CompoundTag.h"

namespace SDK {
    class ItemStackBase {
    public:
        // ItemStackBase
        void** vtable;
        Item** item; // 0x0008 WeakPtr<Item>
        std::unique_ptr<CompoundTag> tag;
        class Block* block; // 0x0018
        short aux;          // 0x0020
        uint8_t itemCount;
        bool valid;
        bool showPickUp;
        bool wasPickedUp;
        std::chrono::steady_clock::time_point pickupTime;
        std::vector<class BlockLegacy*> canPlaceOn;
        uintptr_t canPlaceOnHash;
        std::vector<class BlockLegacy*> canDestroy;
        uintptr_t blockingTick[3]; // Tick

        // TODO: update this

        std::string getHoverName();

        short getDamageValue() {
            return reinterpret_cast<short (*)(ItemStackBase*)>(Signatures::ItemStackBase_getDamageValue.result)(this);
        }

        Item* getItem() {
            if (!item) return nullptr;
            return *item;
        }
    };

    static_assert(sizeof(ItemStackBase) == 0x80);
}
