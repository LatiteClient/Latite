#pragma once
#include <unordered_map>
#include <sdk/common/world/Attribute.h>
#include <optional>
#include <stdexcept>

namespace SDK {
	class BaseAttributeMap {
	public:
		std::unordered_map<unsigned int, AttributeInstance> instances;

		std::optional<AttributeInstance*> getInstance(unsigned int id) {
			try {
				return &instances.at(id);
			}
			catch (std::out_of_range& e) {
				return std::nullopt;
			}
		}
	};
	
	class AttributesComponent {
	public:
		BaseAttributeMap baseAttributes{};
	};
}