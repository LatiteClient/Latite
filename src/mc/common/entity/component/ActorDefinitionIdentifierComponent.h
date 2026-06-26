#pragma once

#include "mc/common/entity/IEntityComponent.h"
#include "mc/deps/core/StringUtils.h"

namespace SDK {
    struct ActorDefinitionIdentifier {
        std::string nameSpace;
        std::string identifier;
        std::string initEvent;
        std::string fullName;
        HashedString canonicalName;
    };

    struct ActorDefinitionIdentifierComponent : IEntityComponent {
        static constexpr uint32_t type_hash = 0xDEB6534F;

        ActorDefinitionIdentifier identifier;
    };
}
