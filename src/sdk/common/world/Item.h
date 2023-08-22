#pragma once
#include <string>
#include "sdk/deps/Core/StringUtils.h"
#include "sdk/Util.h"

namespace SDK {
	class Item {
	public:
		std::string atlas;
		CLASS_FIELD(HashedString, _namespace, 0xB0);
		CLASS_FIELD(HashedString, id, 0xE0);

		int getMaxUseDuration(class ItemStack* item) {
			if (internalVers > V1_19_41) {
				return memory::callVirtual<int>(this, 6, item);
			}
			return memory::callVirtual<int>(this, 4, item);
		}
	};
}