#pragma once
#include "Actor.h"
#include "util/memory.h"

namespace SDK {
	class Mob : public Actor {
	public:
		void setSprinting(bool b) {
			memory::callVirtual<void>(this, 0x8B, b);
		}

		int getItemUseDuration() {
			return memory::callVirtual<int>(this, 0x94);
		}
	};
}