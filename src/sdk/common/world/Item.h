#pragma once
#include <string>
#include "sdk/deps/Core/StringUtils.h"
#include "sdk/Util.h"
#include "api/memory/Memory.h"
#include <sdk/signature/storage.h>

namespace SDK {
	class Item {
	public:
		std::string atlas; // atlas.items
		CLASS_FIELD(std::string, translateName, 0xA8); // item.bow
		CLASS_FIELD(HashedString, id, 0xC8); // bow
		CLASS_FIELD(HashedString, _namespace, 0xF0); // minecraft
		CLASS_FIELD(HashedString, namespacedId, 0x118); // minecraft:bow 

		int getMaxUseDuration(class ItemStackBase* item) {
			if (internalVers >= V1_20_50) {
				return memory::callVirtual<int>(this, 5, item);
			}

			if (internalVers > V1_19_41) {
				return memory::callVirtual<int>(this, 6, item);
			}
			return memory::callVirtual<int>(this, 4, item);
		}

		bool isGlint(class ItemStackBase* item) {
			return memory::callVirtual<bool>(this, mvGetOffset<0x27, 0x26, 0x26, 0x26, 0x32, 0x32, 0x2B, 0x32>(), item);
		}

		int getMaxDamage() {
			return memory::callVirtual<int>(this, mvGetOffset<0x23, 0x22, 0x22, 0x22, 0x2F, 0x2F, 0x28, 0x2F>());
		}

		short getDamageValue(class CompoundTag* tag) {
			// TODO: fix this
			if (internalVers >= V1_20_50) {
				return reinterpret_cast<short(*)(Item*, CompoundTag*)>(Signatures::ItemStackBase_getDamageValue.result)(this, tag);
			}

			return memory::callVirtual<short>(this, mvGetOffset<0x6B, 0x65, 0x6B>(), tag);
		}

		virtual ~Item() = 0;
	};
}