#pragma once
#include <unordered_map>
#include <mc/common/world/Attribute.h>
#include <optional>
#include <stdexcept>

namespace SDK {
	class BaseAttributeMap {
		std::vector<uint32_t> ids;
		std::vector<AttributeInstance> instances;
		char pad[0x28];

		struct AttributeResult { // Made up
			std::vector<uint32_t>::iterator id;
			std::vector<AttributeInstance>::iterator instance;
		};

	public:

		AttributeInstance* getInstance(unsigned int id) {
			using func_t = void(*)(AttributeResult&, BaseAttributeMap*, const uint32_t&);
			static auto func = reinterpret_cast<func_t>(Signatures::BaseAttributeMap_getInstance.result);
			AttributeResult res{};
			func(res, this, id);
			if (res.id == this->ids.end() || res.instance == this->instances.end())
				return nullptr;
			return &*res.instance;
		}
	};
	
	class AttributesComponent {
	public:
		BaseAttributeMap baseAttributes{};
	};
}