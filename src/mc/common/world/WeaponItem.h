#pragma once

#include "Item.h"
#include "ItemTier.h"

namespace SDK {
    class WeaponItem : public Item {
    public:
        CLASS_FIELD(ItemTier const*, tier, 0x218);
    };
}
