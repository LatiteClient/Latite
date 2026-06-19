#pragma once

namespace SDK {
    struct ActorDataFlagComponent : IEntityComponent {
        static constexpr uint32_t type_hash = 0xC67426F3;

        std::bitset<130> flags;
    };
}
