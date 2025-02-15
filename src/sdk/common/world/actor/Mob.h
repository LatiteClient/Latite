#pragma once
#include "Actor.h"
#include "api/memory/memory.h"

namespace SDK {
	class Mob : public Actor {
	public:
		void setSprinting(bool b) {
			memory::callVirtual<void>(this, SDK::mvGetOffset<0x8C, 0x91, 0x92, 0x96, 0x96, 0x99, 0xAF, 0xFB, 0x115, 0x11C>(), b);
		}

		int getItemUseDuration() {
			return memory::callVirtual<int>(this, SDK::mvGetOffset<0x95, 0x9A, 0x9B, 0x9F, 0x9F, 0xA2, 0xB9, 0x109, 0x12A, 0x12F>());
		}
	};
}