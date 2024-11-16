#pragma once
#include <unordered_map>
#include <sdk/common/world/Attribute.h>
#include <optional>
#include <stdexcept>

namespace SDK {
	class BaseAttributeMap {
	public:
		std::unordered_map<unsigned int, AttributeInstance> instances;

	private:
		char pad[0x18];

	public:

		AttributeInstance* getInstance(unsigned int id) {
			using func_t = AttributeInstance&(*)(BaseAttributeMap*, unsigned int);
			static auto func = reinterpret_cast<func_t>(Signatures::BaseAttributeMap_getInstance.result);
			return &func(this, id);
			/*try {
				return &instances.at(id);
			}
			catch (std::out_of_range& e) {
				return std::nullopt;
			}*/
		}
	};
	
	class AttributesComponent {
	public:
		BaseAttributeMap baseAttributes{};
	};
}