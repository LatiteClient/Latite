#pragma once
#include <string>
#include "sdk/deps/Core/StringUtils.h"
#include "sdk/Util.h"
#include "api/memory/Memory.h"

namespace SDK {
	class Item {
	public:
		std::string atlas; // atlas.items
		CLASS_FIELD(std::string, translateName, 0xA8); // item.bow
		CLASS_FIELD(HashedString, id, 0xC8); // bow
		CLASS_FIELD(HashedString, _namespace, 0xF0); // minecraft
		CLASS_FIELD(HashedString, namespacedId, 0x118); // minecraft:bow 

		int getMaxUseDuration(class ItemStack* item) {
			if (internalVers > V1_19_41) {
				return memory::callVirtual<int>(this, 6, item);
			}
			return memory::callVirtual<int>(this, 4, item);
		}

		virtual ~Item() = 0;
	};
}