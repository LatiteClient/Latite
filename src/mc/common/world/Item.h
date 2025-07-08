#pragma once
#include <string>
#include "mc/deps/core/StringUtils.h"
#include "mc/Util.h"
#include "util/memory.h"
#include <mc/Addresses.h>

namespace SDK {
	class Item {
	public:
		CLASS_FIELD(std::string, atlas, 0x10); // atlas.items
		CLASS_FIELD(std::string, translateName, 0xB0); // item.bow
		CLASS_FIELD(HashedString, id, 0xD0); // bow
		CLASS_FIELD(std::string, _namespace, 0x100); // minecraft
		CLASS_FIELD(HashedString, namespacedId, 0x120); // minecraft:bow 

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