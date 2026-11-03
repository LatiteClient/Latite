#pragma once

namespace SDK {
    struct ActorRotationComponent : IEntityComponent {
        static constexpr uint32_t type_hash = 0x75DF36B7;

        Vec2 rotation;
        Vec2 rotationOld;
    };
}
