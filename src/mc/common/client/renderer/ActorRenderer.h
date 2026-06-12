#pragma once

#include "mc/Util.h"

namespace SDK {
	class ActorRenderer {
	public:
		// actor portrait renderer sets this before it draws actors in ui
		CLASS_FIELD(bool, renderingInventory, 0x88);
	};
}
