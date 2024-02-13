#pragma once
#include "Actor.h"
#include "api/memory/memory.h"

namespace SDK {
	class Mob : public Actor {
	public:
		void setSprinting(bool b) {
			// @dump-wbds vtable Mob, setSprinting
			memory::callVirtual<void>(this, SDK::mvGetOffset<0x10D, 0xAF, 0xFB, 0x115, 0x11C>(), b);
		}

		int getItemUseDuration() {
			// @dumdp-wbds vtable Mob, getItemUseDuration
			return memory::callVirtual<int>(this, SDK::mvGetOffset<0xB5, 0xB9, 0x109, 0x12A, 0x12F>());
		}
	};
}