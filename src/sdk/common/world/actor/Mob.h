#pragma once
#include "Actor.h"
#include "api/memory/memory.h"

namespace SDK {
	class Mob : public Actor {
	public:
		void setSprinting(bool b) {
			memory::callVirtual<void>(this, MV_DETAIL_GETOFFSET<0xFB, 0x115, 0x11C>(), b);
		}

		int getItemUseDuration() {
			return memory::callVirtual<int>(this, MV_DETAIL_GETOFFSET<0x109, 0x12A, 0x12F>());
		}
	};
}