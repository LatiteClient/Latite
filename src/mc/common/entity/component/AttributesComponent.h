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
        AttributeInstance* getInstance(Attribute& attribute) {
            if (attribute.mIDValue > 0 && this->ids.size() == this->instances.size()) {
                for (size_t i = 0; i < this->ids.size(); ++i) {
                    if (this->ids[i] == static_cast<uint32_t>(attribute.mIDValue)) {
                        auto* inst = &this->instances[i];
                        auto* attr = *reinterpret_cast<Attribute* const*>(reinterpret_cast<uintptr_t>(inst) + 8);
                        if (attr && attr->mName.hash == attribute.mName.hash) {
                            return inst;
                        }
                    }
                }
            }

            for (size_t i = 0; i < this->instances.size(); ++i) {
                auto* inst = &this->instances[i];
                auto* attr = *reinterpret_cast<Attribute* const*>(reinterpret_cast<uintptr_t>(inst) + 8);
                if (attr && attr->mName.hash == attribute.mName.hash) {
                    attribute.mIDValue = attr->mIDValue;
                    return inst;
                }
            }

            return nullptr;
        }

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
