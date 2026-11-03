#pragma once

namespace SDK {
    struct AABBShapeComponent : IEntityComponent {
        static constexpr uint32_t type_hash = 0xBAC1B3CF;

        AABB boundingBox;
        Vec2 size;
    };
}
