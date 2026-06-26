#pragma once
#include <unordered_map>
#include <mc/common/world/Attribute.h>
#include <optional>
#include <stdexcept>

namespace SDK {
    class BaseAttributeMap {
        std::vector<uint32_t> ids;
        std::vector<AttributeInstance> instances;
        char pad[0x20];

        struct AttributeResult { // Made up
            std::vector<uint32_t>::iterator id;
            std::vector<AttributeInstance>::iterator instance;
        };

    public:
        AttributeInstance* getInstance(unsigned int id) {
            if (this->ids.size() != this->instances.size()) return nullptr;

            for (size_t i = 0; i < this->ids.size(); i++) {
                if (this->ids[i] == id) return &this->instances[i];
            }

            return nullptr;
        }
    };

    struct AttributesComponent : IEntityComponent {
        static constexpr uint32_t type_hash = 0xFD3B0613;

        BaseAttributeMap baseAttributes {};
    };
}
