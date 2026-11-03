#pragma once

namespace SDK {
    struct ActorEquipmentComponent : IEntityComponent {
        static constexpr uint32_t type_hash = 0xB06141A9;

        SDK::Inventory* handContainer;
        SDK::Inventory* armorContainer;
    };
}
