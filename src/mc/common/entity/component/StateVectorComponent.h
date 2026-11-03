#pragma once

namespace SDK {
    struct StateVectorComponent : IEntityComponent {
        static constexpr uint32_t type_hash = 0x1B5D5238;

        Vec3 pos;
        Vec3 posOld;
        Vec3 velocity;
    };
}
