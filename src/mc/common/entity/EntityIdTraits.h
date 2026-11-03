#pragma once

#include "EntityId.h"

struct EntityIdTraits {
    using value_type = EntityId;

    using entity_type = uint32_t;
    using version_type = uint16_t;

    static constexpr entity_type entity_mask = 0x3FFFF; // lower 18 bits of raw id
    static constexpr entity_type version_mask = 0x3FFF; // upper 14 bits of raw id
};
