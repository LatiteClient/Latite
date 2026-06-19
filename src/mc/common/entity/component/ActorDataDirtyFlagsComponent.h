#pragma once

namespace SDK {
    struct ActorDataDirtyFlagsComponent : IEntityComponent {
        static constexpr uint32_t type_hash = 0xC131F6A8;

        std::bitset<141> flags;
    };
}
