#pragma once

namespace SDK {
    struct RuntimeIDComponent : IEntityComponent {
        static constexpr uint32_t type_hash = 0xFC0DBBB5;

        uint64_t runtimeID;
    };
}
